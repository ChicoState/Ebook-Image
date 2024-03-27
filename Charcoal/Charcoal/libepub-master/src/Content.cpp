/*
Copyright (c) 2014, Richard Martin
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Richard Martin nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL RICHARD MARTIN BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "Content.hpp"

#include <utility>
#include <boost/filesystem.hpp>
#include <libxml++/libxml++.h>
#include <exception>

#include "SQLiteUtils.hpp"

using std::move;
using std::pair;
using std::string;

#ifdef DEBUG
#include <iostream>
using std::cout;
using std::endl;
#endif

using namespace boost::filesystem;
using namespace xmlpp;

ContentItem::ContentItem(ContentType _type, CSSRule _rule, path _file, ustring _id, ustring _content, ustring _stripped_content) :
	type(_type),
	rule(_rule),
	file(_file),
	id(_id),
	content(_content),
	stripped_content(_stripped_content)
{
}

ContentItem::ContentItem(ContentItem const & cpy)  :
	type(cpy.type),
	rule(cpy.rule),
	file(cpy.file),
	id(cpy.id),
	content(cpy.content),
	stripped_content(cpy.stripped_content)
{
}

ContentItem::ContentItem(ContentItem && mv)  :
	type(move(mv.type)),
	rule(move(mv.rule)),
	file(move(mv.file)),
	id(move(mv.id)),
	content(move(mv.content)),
	stripped_content(move(mv.stripped_content))
{
}

ContentItem & ContentItem::operator =(const ContentItem & cpy)
{
	type = cpy.type;
	rule = cpy.rule;
	file = cpy.file;
	id = cpy.id;
	content = cpy.content;
	stripped_content = cpy.stripped_content;
	return *this;
}

ContentItem & ContentItem::operator =(ContentItem && mv)
{
	type = move(mv.type);
	rule = move(mv.rule);
	file = move(mv.file);
	id = move(mv.id);
	content = move(mv.content);
	stripped_content = move(mv.stripped_content);
	return *this;
}

ContentItem::~ContentItem()
{
}

///////////////////////
namespace {

	inline ustring __create_text(const ustring & nodename, const ustring & nodecontents)
	{
		ustring tmp;
		tmp += "<";
		tmp += nodename;
		tmp += ">";
		tmp += nodecontents;
		tmp += "</";
		tmp += nodename;
		tmp += ">";
		return tmp;
	}

	ustring __id = "";

	string i_key;
	string b_key;
	string big_key;
	string s_key;
	string sub_key;
	string sup_key;
	string small_key;
	string tt_key;
	string u_key;
	string a_key;
	string span_key;
	string class_key;
	string hr_key;
	string p_key;
	string h1_key;
	string h2_key;
	string id_key;
	string _blank_key;

	inline CSSRule __find_css(const Element * const childElement, const CSS & css)
	{

		CSSRule rule;

		ustring id_name = "";
		ustring class_name = "";
		ustring element_name = childElement->get_name();

		const auto attributes = childElement->get_attributes();

		for(auto iter = attributes.begin(); iter != attributes.end(); ++iter) {

			const Attribute * attribute = *iter;

			if(attribute->get_name().collate_key() == class_key) {
				//We've found a class here.
				class_name = attribute->get_value();
			}
			else if (attribute->get_name().collate_key() == class_key) {
				//We've found in id here.
				id_name = attribute->get_value();
			}

		}

		if(!id_name.empty()) {

			//Look for rules of the type p#id;

			ustring tmp = "";
			tmp += element_name;
			tmp += "#";
			tmp += id_name;

			if(css.contains_rule(tmp)) {
				rule.add(css.get_rule(tmp));
			}

			tmp = "#";
			tmp += id_name;

			//Look for simple #id rules.

			if(css.contains_rule(tmp)) {
				rule.add(css.get_rule(tmp));
			}

		}

		if(!class_name.empty()) {

			//Look for rules of the type p.class;

			ustring tmp = "";
			tmp += element_name;
			tmp += ".";
			tmp += class_name;

			if(css.contains_rule(tmp)) {
				rule.add(css.get_rule(tmp));
			}

			tmp = ".";
			tmp += class_name;

			//Look for simple .class rules.

			if(css.contains_rule(tmp)) {
				rule.add(css.get_rule(tmp));
			}

		}

		if(css.contains_rule(element_name)) {
			rule.add(css.get_rule(element_name));
		}

		return rule;

	}

	//This whole method is fairly awful.
	pair<ustring, ustring> __recursive_strip(vector<ContentItem> & items, const CSS & css, const path & file, const Node * const node)
	{

		const TextNode * nodeText = dynamic_cast<const TextNode *>(node);

		if(nodeText) {
			if(nodeText->is_white_space()) {
				return pair<ustring, ustring>("", "");
			}

			ustring content = nodeText->get_content();
			return pair<ustring, ustring>(content, content);
		}

		const Element * nodeElement = dynamic_cast<const Element *>(node);
		const auto nodelist = nodeElement->get_children();

		ustring value = "";
		ustring value_stripped = "";

		for(auto niter = nodelist.begin(); niter != nodelist.end(); ++niter) {

			const Node * childNode = *niter;

			//Still still Genuinely horrible.
			const Element * childElement = dynamic_cast<const Element *>(childNode);
			const TextNode * childText = dynamic_cast<const TextNode *>(childNode);

			if(!childElement && !childText) {
				continue;
			}

			if(childText) {
				pair<ustring, ustring> res = __recursive_strip(items, css, file, childNode);
				value += res.first;
				value_stripped += res.second;
			}

			if(childElement) {

				const ustring childname = childElement->get_name();
				const string childname_key = childname.collate_key();

				if(childname_key == i_key) {
					//italic.
					pair<ustring, ustring> res = __recursive_strip(items, css, file, childNode);
					value += __create_text("i", res.first);
					value_stripped += res.second;
				}
				else if(childname_key == b_key) {
					//bold;
					pair<ustring, ustring> res = __recursive_strip(items, css, file, childNode);
					value += __create_text("b", res.first);
					value_stripped += res.second;
				}
				else if(childname_key == big_key) {
					pair<ustring, ustring> res = __recursive_strip(items, css, file, childNode);
					value += __create_text("big", res.first);
					value_stripped += res.second;
				}
				else if(childname_key == s_key) {
					//strikethrough
					pair<ustring, ustring> res = __recursive_strip(items, css, file, childNode);
					value += __create_text("s", res.first);
					value_stripped += res.second;
				}
				else if(childname_key == sub_key) {
					pair<ustring, ustring> res = __recursive_strip(items, css, file, childNode);
					value += __create_text("sub", res.first);
					value_stripped += res.second;
				}
				else if(childname_key == sup_key) {
					pair<ustring, ustring> res = __recursive_strip(items, css, file, childNode);
					value += __create_text("sup", res.first);
					value_stripped += res.second;
				}
				else if(childname_key == small_key) {
					pair<ustring, ustring> res = __recursive_strip(items, css, file, childNode);
					value += __create_text("i", res.first);
					value_stripped += res.second;
				}
				else if(childname_key == tt_key) {
					//monospace
					pair<ustring, ustring> res = __recursive_strip(items, css, file, childNode);
					value += __create_text("tt", res.first);
					value_stripped += res.second;
				}
				else if(childname_key == u_key) {
					//underline
					pair<ustring, ustring> res = __recursive_strip(items, css, file, childNode);
					value += __create_text("u", res.first);
					value_stripped += res.second;
				}
				else if(childname_key == a_key) {
					//specific bheaviour for stripping hyperlinks within the text.
					//I suspect that I'll have to come back to this, but at the moment I'm
					//not completely sure how to handle it.
					pair<ustring, ustring> res = __recursive_strip(items, css, file, childNode);
					value += res.first;
					value_stripped += res.second;
				}
				else if(childname_key == span_key) {
					//specific bheaviour for stripping span tags
					//Try to find a class attribute.
					CSSRule tmp;
					const auto attributes = childElement->get_attributes();

					for(auto iter = attributes.begin(); iter != attributes.end(); ++iter) {

						const Attribute * attribute = *iter;

						if(attribute->get_name().collate_key() == class_key) {
							//We've found a class here.
							ustring cname = attribute->get_value();
							//Need to do this better in the future:
							tmp = css.get_rule(ustring(".") + cname);
						}
					}

					pair<ustring, ustring> res = __recursive_strip(items, css, file, childNode);

					if(tmp.fontweight == FONTWEIGHT_BOLD) {
						value += __create_text("b", res.first);
					}
					else if (tmp.fontstyle == FONTSTYLE_ITALIC) {
						value += __create_text("i", res.first);
					}
					else {
						value += res.first;
					}

					value_stripped += res.second;

				}
				else if(childname_key == hr_key)  {
					//What to do if this is a nested <hr> tag within (frequently)
					//a <p> tag.
					ContentType ct = HR;

					//See if we can find a CSS class for this.
					CSSRule rule = css.get_rule("hr");

					//Add it directly to the items:
					items.emplace_back(ct, rule, file, __id, "", "");

					value = "";
					value_stripped = "";
				}
			}
		}

		return pair<ustring, ustring>(value, value_stripped);
	}

	void __recursive_find(vector<ContentItem> & items, const CSS & css, const path & file, const Node * const node)
	{
		const auto nlist = node->get_children();

		for(auto niter = nlist.begin(); niter != nlist.end(); ++niter) {

			const Node * ntmp = *niter;

			//Still still Genuinely horrible.
			const Element * tmpnode = dynamic_cast<const Element *>(ntmp);

			if(!tmpnode) {
				continue;
			}

			const ustring tmpnodename = tmpnode->get_name();
			const string name_key = tmpnodename.collate_key();

			if(name_key != p_key && name_key !=  h1_key && name_key != h2_key && name_key != hr_key) {
				__recursive_find(items, css, file, ntmp);
			}
			else {

				ContentType ct = P;
				//Try to get a class for the content type.
				CSSRule rule;

				if(name_key == p_key) {
					ct = P;
					rule = css.get_rule("p");
				}
				else if (name_key == h1_key) {
					ct = H1;
					rule = css.get_rule("h1");
				}
				else if (name_key == h2_key) {
					ct = H2;
					rule = css.get_rule("h2");
				}
				else if (name_key == hr_key) {
					ct = HR;
					rule = css.get_rule("hr");
				}

				const auto attributes = tmpnode->get_attributes();

				for(auto iter = attributes.begin(); iter != attributes.end(); ++iter) {
					const Attribute * attribute = *iter;

					const string attr_key = attribute->get_name().collate_key();

					if(attr_key == id_key) {
						__id = attribute->get_value();
					}

					/* TODO!!!!!
					if(attr_key == class_key) {
						//We've found an additional class here.
						ustring cname = attribute->get_value();
						CSSClass tmp = classes.get_class(cname);
						cssclass.add(tmp);
					}
					*/
				}

				pair<ustring, ustring> content = __recursive_strip(items, css, file, ntmp);

				if(ct != HR && content.first.empty()) {
					continue;
				}

				#ifdef DEBUG
				cout << tmpnode->get_name()  << " " << __id << endl;
				cout << " \t " << content.first << endl;
				cout << " \t " << content.second << endl;
				#endif
				items.emplace_back(ct, rule, file, __id, content.first, content.second);

			}
		}
	}
} // end anonymous namespace

Content::Content(CSS & _css, vector<path> _files) :
	css(_css),
	files(_files),
	items()
{

	items.reserve(2000);

	for(const auto file : files) {

		__id = "";

		if(!exists(file)) {
			throw std::runtime_error("Content file specified in OPF file does not exist!");
		}

		#ifdef DEBUG
		cout << "Loading content file " << file << endl;
		#endif

		DomParser parser;
		parser.parse_file(file.string());

		//The DomParser futzes with the locale,
		//which means that the collation keys
		//created _before_ this will be incompatible
		//with the collation keys created during the
		//recursive methods.
		//
		//So, setup the keys here.
		i_key = ustring("i").collate_key();
		b_key = ustring("b").collate_key();
		big_key = ustring("big").collate_key();
		s_key = ustring("s").collate_key();
		sub_key = ustring("sub").collate_key();
		sup_key = ustring("sup").collate_key();
		small_key = ustring("small").collate_key();
		tt_key = ustring("tt").collate_key();
		u_key = ustring("u").collate_key();
		a_key = ustring("a").collate_key();
		span_key = ustring("span").collate_key();
		class_key = ustring("class").collate_key();
		hr_key = ustring("hr").collate_key();
		p_key = ustring("p").collate_key();
		h1_key = ustring("h1").collate_key();
		h2_key = ustring("h2").collate_key();
		id_key = ustring("id").collate_key();
		_blank_key = ustring ("").collate_key();

		const Node * root = parser.get_document()->get_root_node();
		const ustring rootname = root->get_name();

		if(rootname.compare("html") != 0) {
			throw std::runtime_error("Linked content file isn't HTML. So we can't read it. Mostly through laziness.");
		}

		const auto nlist = root->get_children();

		for(auto niter = nlist.begin(); niter != nlist.end(); ++niter) {

			const Node * ntmp = *niter;
			//Still still Genuinely horrible.
			const Element * tmpnode = dynamic_cast<const Element *>(ntmp);

			if(!tmpnode) {
				continue;
			}

			if(tmpnode->get_name().compare("body") == 0)  {
				__recursive_find(items, _css, file, ntmp);
			}

		}
	}
}

Content::Content(CSS & _css, sqlite3 * const db, const unsigned int epub_file_id, const unsigned int opf_index) :
	css(_css),
	files(),
	items()
{

	items.reserve(2000);

	int rc;

	const string content_select_sql = "SELECT * FROM content WHERE epub_file_id=? AND opf_id=?;";

	sqlite3_stmt * content_select;

	rc = sqlite3_prepare_v2(db, content_select_sql.c_str(), -1, &content_select, 0);

	if(rc != SQLITE_OK && rc != SQLITE_DONE) {
		throw - 1;
	}

	sqlite3_bind_int(content_select, 1, epub_file_id);
	sqlite3_bind_int(content_select, 2, opf_index);

	rc = sqlite3_step(content_select);

	while ( rc == SQLITE_ROW ) {

		ContentType type = (ContentType) sqlite3_column_int(content_select, 3);
		CSSRule rule = _css.get_rule(sqlite3_column_ustring(content_select, 4));
		path file(sqlite3_column_string(content_select, 5));
		ustring id = sqlite3_column_ustring(content_select, 6);
		ustring content = sqlite3_column_ustring(content_select, 7);
		ustring stripped_content = sqlite3_column_ustring(content_select, 8);

		items.emplace_back(type, rule, file, id, content, stripped_content);

		rc = sqlite3_step(content_select);

	}

	sqlite3_finalize(content_select);

}

Content::Content(Content const & cpy) :
	css(cpy.css),
	files(cpy.files),
	items(cpy.items)
{
}

Content::Content(Content && mv) :
	css(mv.css),
	files(move(mv.files)),
	items(move(mv.items))
{
}

Content & Content::operator =(const Content & cpy)
{
	css = cpy.css;
	files = cpy.files;
	items = cpy.items;
	return *this;
}

Content & Content::operator =(Content && mv)
{
	css = mv.css;
	files = move(mv.files);
	items = move(mv.items);
	return *this;
}

Content::~Content()
{
}

void Content::save_to(sqlite3 * const db, const unsigned int epub_file_id, const unsigned int opf_index)
{

	int rc;
	char * errmsg;

	const string content_table_sql = "CREATE TABLE IF NOT EXISTS content("  \
	                                 "content_id 				INTEGER PRIMARY KEY," \
	                                 "epub_file_id			INTEGER NOT NULL," \
	                                 "opf_id 				INTEGER NOT NULL," \
	                                 "type			 		INTEGER NOT NULL," \
	                                 "css_selector		 		TEXT NOT NULL," \
	                                 "filename	 			TEXT NOT NULL," \
	                                 "id			 		TEXT NOT NULL," \
	                                 "content	 			TEXT NOT NULL," \
	                                 "stripped_content		TEXT NOT NULL) ;";
	sqlite3_exec(db, content_table_sql.c_str(), NULL, NULL, &errmsg);

	//Tables created.

	sqlite3_stmt * content_insert;

	const string content_insert_sql = "INSERT INTO content (epub_file_id, opf_id, type, css_selector, filename, id, content, stripped_content) VALUES (?, ?, ?, ?, ?, ?, ?, ?);";

	rc = sqlite3_prepare_v2(db, content_insert_sql.c_str(), -1, &content_insert, 0);

	if(rc != SQLITE_OK && rc != SQLITE_DONE) {
		throw - 1;
	}

	for(auto & contentitem : items) {

		sqlite3_bind_int(content_insert, 1, epub_file_id);
		sqlite3_bind_int(content_insert, 2, opf_index);
		sqlite3_bind_int(content_insert, 3, (int) contentitem.type);
		sqlite3_bind_text(content_insert, 4, contentitem.rule.selector.raw_text.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(content_insert, 5, contentitem.file.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(content_insert, 6,  contentitem.id.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(content_insert, 7, contentitem.content.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(content_insert, 8, contentitem.stripped_content.c_str(), -1, SQLITE_STATIC);

		int result = sqlite3_step(content_insert);

		if(result != SQLITE_OK && result != SQLITE_ROW && result != SQLITE_DONE) {
			throw - 1;
		}

		sqlite3_reset(content_insert);

	}

	sqlite3_finalize(content_insert);

	//Create an index
	const string content_index_sql = "CREATE INDEX index_content ON content(epub_file_id, opf_id);";
	sqlite3_exec(db, content_index_sql.c_str(), NULL, NULL, &errmsg);

}
