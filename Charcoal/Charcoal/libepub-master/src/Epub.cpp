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

#include "Epub.hpp"

#include <utility>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <locale>
#include <sys/types.h>
#include <sys/wait.h>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/functional/hash.hpp>

#include "SQLiteUtils.hpp"

using std::string;
using std::move;
using std::size_t;
using std::stringstream;
using std::locale;
using std::hex;
using std::ifstream;
using namespace boost::filesystem;
using boost::lexical_cast;
using boost::hash_combine;

#ifdef DEBUG
#include <iostream>
using std::cout;
using std::endl;
#endif

Epub::Epub(string _filename) :
	from_epub(true),
	filename(_filename)
{
	//check if the file exists first.
	if(!exists(filename)) {
		throw std::runtime_error("No such filename");
	}

	//setup the absolute path
	absolute_path = absolute(filename);

	//It does exist.
	//Compute the hash
	hash = compute_epub_hash(absolute_path);
	stringstream stream;
	//Have to set the locale on the stringstream
	//to "C" otherwise it does insane things like
	//formatting hex with decimal comma groups.
	//Yeah. About that.
	locale cloc("C");
	stream.imbue(cloc);
	stream << std::hex << (unsigned long) hash;
	hash_string = stream.str();
	#ifdef DEBUG
	cout << "\t Hash: " << hash_string << endl;
	#endif

	//Now to do work.
	path to_tmp = temp_directory_path();
	#ifdef DEBUG
	cout << "Temporary path is " << to_tmp << endl;
	#endif
	to_tmp /= "epub";
	create_directory(to_tmp);

	to_tmp /= hash_string;
	directory_path = to_tmp;

	#ifdef DEBUG
	cout << "Temporary directory is " << to_tmp << endl;
	#endif

	create_directory(directory_path);

	//OK, so we have some stuff to play with. Now. Time to inflate.
	//This is a hack and I bloody hate it. But it'll suffice for now.
	const char * program_path = "/usr/bin/unzip";
	const char * program_name = "unzip";
	auto pid = fork();

	if (pid == -1) {
		throw std::runtime_error("Some problem with fork()");
	}
	else if (pid == 0) {
		const char * argv0 = program_name;
		const char * argv1 = "-qq";
		const char * argv2 = filename.c_str();
		const char * argv3 = "-d";
		const char * argv4 = directory_path.string().c_str();
		execl(program_path, argv0, argv1, argv2, argv3, argv4, NULL);
		throw std::runtime_error("Some problem with execl");
	}
	else {
		int status;

		#ifdef DEBUG
		cout << "Waiting for pid " << pid << endl;
		#endif

		waitpid(pid, &status, 0);

		#ifdef DEBUG
		cout << "PID joined " << pid << endl;
		#endif

	}

	path to_mimetype = to_tmp;
	to_mimetype /= "mimetype";
	ifstream mimetypefile (to_mimetype.string());

	if(mimetypefile.is_open()) {
		string line;
		getline(mimetypefile, line);
		const string target = "application/epub+zip";
		int res = line.compare(target);

		if(res != 0) {
			throw std::runtime_error("mimetype file present, but invalid");
		}
	}
	else {
		throw std::runtime_error("No mimetype file, is this an epub?");
	}

	path to_container = to_tmp;
	to_container /= "META-INF";
	to_container /= "container.xml";

	if(!exists(to_container)) {
		throw std::runtime_error("container.xml does not exist within META-INF dir");
	}

	//OK, file is validated and unpacked in a temporary directory
	container.load(directory_path);

	for(auto rf : container.rootfiles) {

		OPF tmp(directory_path, rf.full_path);
		opf_files.push_back(tmp);

		vector<path> cssfiles;

		for(ManifestItem mi : tmp.find_manifestitems_by_type("text/css")) {
			path tmp(rf.full_path);
			path parent = tmp.parent_path();
			path cssfile = directory_path;
			cssfile /= parent;
			cssfile /= path(mi.href);
			cssfiles.push_back(cssfile);
		}

		CSS cssclasses(cssfiles);
		css.push_back(cssclasses);

		vector<path> contentfiles;

		for(auto si : tmp.spine) {
			ustring idref = si.idref;
			ManifestItem item = tmp.find_manifestitem_by_id(idref);
			path tmp(rf.full_path);
			path parent = tmp.parent_path();
			path contentfile = directory_path;
			contentfile /= parent;
			contentfile /= path(item.href);
			contentfiles.push_back(contentfile);
		}

		Content content(cssclasses, contentfiles);
		contents.push_back(content);

	}
}

Epub::Epub(sqlite3 * const db, const unsigned int file_id) : from_epub(false)
{

	int rc;

	const string files_select_sql = "SELECT * FROM epub_files WHERE epub_file_id=?;";

	sqlite3_stmt * files_select;

	rc = sqlite3_prepare_v2(db, files_select_sql.c_str(), -1, &files_select, 0);

	if(rc != SQLITE_OK && rc != SQLITE_DONE) {
		throw - 1;
	}

	sqlite3_bind_int(files_select, 1, file_id);

	rc = sqlite3_step(files_select);

	if(rc != SQLITE_ROW) {
		//Can't find that file ID.
		throw - 1;
	}

	filename = path(sqlite3_column_string(files_select, 1));
	absolute_path = path(sqlite3_column_string(files_select, 2));
	hash = (size_t) sqlite3_column_int64(files_select, 3);
	hash_string = sqlite3_column_string(files_select, 4);

	sqlite3_finalize(files_select);

	container.load(db, file_id);

	//There should be an opf file for each root file.

	unsigned int i = 0;
	unsigned int n_opf = container.rootfiles.size();

	for( ; i < n_opf; i++) {
		OPF tmp(db, file_id, i);
		opf_files.push_back(tmp);

		CSS cssclasses(db, file_id, i);
		css.push_back(cssclasses);
	}

}

size_t inline Epub::compute_epub_hash(const path & _absolute_path)
{

	unsigned int size = file_size(_absolute_path);
	time_t lmtime = last_write_time(_absolute_path);
	string timestring = lexical_cast<std::string>(lmtime);

	#ifdef DEBUG
	cout << "File Exists" << endl;
	cout << "\t Name: " << _absolute_path.filename().string() << endl;
	cout << "\t Absolute path: " << _absolute_path.string() << endl;
	cout << "\t Size: " << size << endl;
	cout << "\t Last Modified: " << timestring << endl;
	#endif

	//Calculate the hash.
	size_t filehash = 0;
	hash_combine<string>(filehash, _absolute_path.string());
	hash_combine<unsigned int>(filehash, size);
	hash_combine<string>(filehash, timestring);

	return filehash;
}

Epub::Epub(Epub const & cpy) :
	from_epub(cpy.from_epub),
	filename(cpy.filename),
	absolute_path(cpy.absolute_path),
	hash(cpy.hash),
	hash_string(cpy.hash_string),
	directory_path(cpy.directory_path),
	container(cpy.container),
	opf_files(cpy.opf_files),
	contents(cpy.contents)
{
}

Epub::Epub(Epub && mv)  :
	from_epub(move(mv.from_epub)),
	filename (move(mv.filename)),
	absolute_path(move(mv.absolute_path)),
	hash(move(mv.hash)),
	hash_string(move(mv.hash_string)),
	directory_path(move(mv.directory_path)),
	container(move(mv.container)),
	opf_files(move(mv.opf_files)),
	contents(move(mv.contents))
{
}

Epub & Epub::operator =(const Epub & cpy)
{
	from_epub = cpy.from_epub;
	filename = cpy.filename;
	absolute_path = cpy.absolute_path;
	hash = cpy.hash;
	hash_string = cpy.hash_string;
	directory_path = cpy.directory_path;
	container = cpy.container;
	opf_files = cpy.opf_files;
	contents = cpy.contents;
	return *this;
}

Epub & Epub::operator =(Epub && mv)
{
	from_epub = move(mv.from_epub);
	filename = move(mv.filename);
	absolute_path = move(mv.absolute_path);
	hash = move(mv.hash);
	hash_string = move(mv.hash_string);
	directory_path = move(mv.directory_path);
	container = move(mv.container);
	opf_files = move(mv.opf_files);
	contents = move(mv.contents);
	return *this;
}

Epub::~Epub()
{
	if(from_epub) {
		#ifdef DEBUG
		cout << "Cleaning up EpubFile in dir" << directory_path << endl;
		#endif
		remove_all(directory_path);
	}
}

void Epub::save_to(sqlite3 * const db)
{
	int rc;
	char * errmsg;

	//Do all the following inserts in an SQLite Transaction, because this speeds up the inserts like crazy.
	sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &errmsg);

	//First, write a little high-level information to the database.
	const string table_sql = "CREATE TABLE IF NOT EXISTS epub_files("  \
	                         "epub_file_id INTEGER PRIMARY KEY," \
	                         "filename TEXT NOT NULL," \
	                         "absolute_path TEXT NOT NULL," \
	                         "hash INTEGER NOT NULL," \
	                         "hash_string TEXT NOT NULL) ;";
	sqlite3_exec(db, table_sql.c_str(), NULL, NULL, &errmsg);
	//Table created.

	const string files_insert_sql = "INSERT INTO epub_files (filename, absolute_path, hash, hash_string) VALUES (?, ?, ?, ?);";
	sqlite3_stmt * files_insert;
	rc = sqlite3_prepare_v2(db, files_insert_sql.c_str(), -1, &files_insert, 0);

	if(rc != SQLITE_OK && rc != SQLITE_DONE) {
		throw - 1;
	}

	sqlite3_bind_text(files_insert, 1, filename.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(files_insert, 2, absolute_path.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_int64(files_insert, 3, hash);
	sqlite3_bind_text(files_insert, 4, hash_string.c_str(), -1, SQLITE_STATIC);

	int result = sqlite3_step(files_insert);

	if(result != SQLITE_OK && result != SQLITE_ROW && result != SQLITE_DONE) {
		throw - 1;
	}

	//get the new id:
	const auto key = sqlite3_last_insert_rowid(db);

	sqlite3_finalize(files_insert);

	container.save_to(db, key);
	unsigned int index = 0;

	for(auto & opf : opf_files) {
		opf.save_to(db, key, index++);
	}

	index = 0;

	for(auto & cssclasses : css) {
		cssclasses.save_to(db, key, index++);
	}

	index = 0;

	for(auto & content : contents) {
		content.save_to(db, key, index++);
	}

	sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &errmsg);

}

