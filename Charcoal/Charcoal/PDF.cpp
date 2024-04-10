#include "PDF.h"
#include <mupdf/fitz.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
book PDF::add(PWSTR path) {
	book curr;
	std::string npath = wstrtostr(path);
	const char* filename = npath.c_str();
	// Open the PDF document
	fz_context* ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
	fz_document* doc = fz_open_document(ctx, filename);
	if (!doc) {
	    std::cerr << "failed to open pdf document." << std::endl;
	    fz_drop_context(ctx);
	    return curr; // return empty string or handle error as needed
	}

	// get document metadata
	char buffer[256]; // buffer to store metadata string

	// retrieve title metadata
	int title_len = fz_lookup_metadata(ctx, doc, "title", buffer, sizeof(buffer));
	if (title_len > 0) {
	    std::string title(buffer, title_len);
	    curr.title = title;
	}
	else {
	    std::cerr << "title metadata not found or too large to fit in buffer." << std::endl;
	}

	// you can retrieve other metadata similarly
	// e.g., author, publisher, etc.

	// clean up
	fz_drop_document(ctx, doc);
	fz_drop_context(ctx);

	collection.push_back(curr);
	return curr;

}

