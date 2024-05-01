#include "Charcoal.h"
#include<windows.h>
#include<shobjidl.h>
#include<shlobj.h>
#include<shlwapi.h>
#include<KnownFolders.h>
#include<shtypes.h>
#include<propsys.h>
#include<commdlg.h>
#include <string.h>
#include <Ultralight/Ultralight.h>
#include <Appcore/AppCore.h>
#include <JavaScriptCore/JavaScript.h>
#include "Library.h"
#include <iostream>
#include <JavaScriptCore/JSRetainPtr.h>

#define WINDOW_WIDTH  900
#define WINDOW_HEIGHT 600


RefPtr<JSContext> context;

using namespace ultralight;
Library ebooks;
Charcoal::Charcoal() {
    ///
    /// Create our main App instance.
    ///
    app_ = App::Create();

    ///
    /// Create a resizable window by passing by OR'ing our window flags with
    /// kWindowFlags_Resizable.
    ///
    window_ = Window::Create(app_->main_monitor(), WINDOW_WIDTH, WINDOW_HEIGHT,
        false, 2 | 4);

    window_->SetTitle("Charcoal");
    ///
    /// Create our HTML overlay-- we don't care about its initial size and
    /// position because it'll be calculated when we call OnResize() below.
    ///
    overlay_ = Overlay::Create(window_, 1, 1, 0, 0);

    ///
    /// Force a call to OnResize to perform size/layout of our overlay.
    ///
    OnResize(window_.get(), window_->width(), window_->height());

    ///
    /// Load a page into our overlay's View
    ///
    overlay_->view()->LoadURL("file:///app.html");

    ///
    /// Register our MyApp instance as an AppListener so we can handle the
    /// App's OnUpdate event below.
    ///
    app_->set_listener(this);

    ///
    /// Register our MyApp instance as a WindowListener so we can handle the
    /// Window's OnResize event below.
    ///
    window_->set_listener(this);

    ///
    /// Register our MyApp instance as a LoadListener so we can handle the
    /// View's OnFinishLoading and OnDOMReady events below.
    ///
    overlay_->view()->set_load_listener(this);

    ///
    /// Register our MyApp instance as a ViewListener so we can handle the
    /// View's OnChangeCursor and OnChangeTitle events below.
    ///
    overlay_->view()->set_view_listener(this);
}


Charcoal::~Charcoal() {
}

void Charcoal::deleteBooks(const JSObject& thisObject, const JSArgs& args) {

}

JSValue Charcoal::printAllBooks(const JSObject& thisObject, const JSArgs& args) {
   
    std::string bookList = ebooks.printall();

    MessageBoxA(NULL, bookList.c_str(), "Book List", MB_OK); //this confirms there is an actual booklist.

    //convert the book list string to a JavaScript string, but does not work. 

    JSStringRef jsBookList = JSStringCreateWithUTF8CString(bookList.c_str());

    JSValue jsValue = JSValue(jsBookList);

    JSStringRelease(jsBookList);

    //return the JSValue object, I tried with JSString and the issue persisted. 

    return jsValue;

}


JSValueRef OnButtonClick(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount,
    const JSValueRef arguments[], JSValueRef* exception) { //LISTS BOOKS

    std::string bookList = ebooks.printall();
    std::string str = "document.getElementById('List2').innerHTML = (";

    str += bookList;
    str += ");";
    const char* ct = str.c_str();
    
    MessageBoxA(NULL, ct, "Book List2", MB_OK);
    
    // Create our list with JavaScript
    JSStringRef script = JSStringCreateWithUTF8CString(ct);

    // Execute it with JSEvaluateScript, ignoring other parameters for now
    JSEvaluateScript(ctx, script, 0, 0, 0, 0);

    // Release our string (we only Release what we Create)
    JSStringRelease(script);



    return JSValueMakeNull(ctx);
}

void Charcoal::grayscaleName(const JSObject& thisObject, const JSArgs& args) { //at the moment just gets metadata, Thomas will puts grayscale hooks here.
    //std::string added = ebooks.getStringData();
    //MessageBoxA(NULL, added.c_str(), "Book Data", MB_OK);
}


void Charcoal::OpenFile(const JSObject& thisObject, const JSArgs& args)
{
    //dialog box for file select
    PWSTR pszFilePath = NULL;
    IFileOpenDialog* pFileOpen;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
    if (SUCCEEDED(hr))
    {
        hr = pFileOpen->Show(NULL);
        if (SUCCEEDED(hr))
        {
            IShellItem* pItem;
            hr = pFileOpen->GetResult(&pItem);
            if (SUCCEEDED(hr))
            {

                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                if (SUCCEEDED(hr))
                {
                    
                    CoTaskMemFree(pszFilePath);
                }
                pItem->Release();
            }
            else
            {
                pFileOpen->Release();
                return;
            }
        }
        else
        {
            pFileOpen->Release();
            return;
        }
        pFileOpen->Release();
    }
    //add selected file to library
    std::string title = ebooks.add(pszFilePath);
    if (title != "")
    {
        std::string added = ebooks.getStringData(title);
        MessageBoxA(NULL, added.c_str(), "Book Added", MB_OK);
    }
    return;
}

void Charcoal::OnDOMReady(ultralight::View* caller,
    uint64_t frame_id,
    bool is_main_frame,
    const String& url) {
    context = caller->LockJSContext();
    SetJSContext(context->ctx());
    JSObject global = JSGlobalObject();
    global["AddBook"] = BindJSCallback(&Charcoal::OpenFile);
    global["nameToGrayscale"] = BindJSCallback(&Charcoal::grayscaleName);
    global["deleteBook"] = BindJSCallback(&Charcoal::deleteBooks);
    
    auto scoped_context = context;

    // Typecast to the underlying JSContextRef.
    JSContextRef ctx = (*scoped_context);

    // Create a JavaScript String containing the name of our callback.
    JSStringRef name = JSStringCreateWithUTF8CString("OnButtonClick");

    // Create a garbage-collected JavaScript function that is bound to our
    // native C callback 'OnButtonClick()'.
    JSObjectRef func = JSObjectMakeFunctionWithCallback(ctx, name, OnButtonClick);

    // Get the global JavaScript object (aka 'window')
    JSObjectRef globalObj = JSContextGetGlobalObject(ctx);

    // Store our function in the page's global JavaScript object so that it
    // accessible from the page as 'OnButtonClick()'.
    JSObjectSetProperty(ctx, globalObj, name, func, 0, 0);

    // Release the JavaScript String we created earlier.
    JSStringRelease(name);


}


void Charcoal::Run() {
    app_->Run();
}

void Charcoal::OnUpdate() {
    ///
    /// This is called repeatedly from the application's update loop.
    ///
    /// You should update any app logic here.
    ///
}

void Charcoal::OnClose(ultralight::Window* window) {
    app_->Quit();
}

void Charcoal::OnResize(ultralight::Window* window, uint32_t width, uint32_t height) {
    ///
    /// This is called whenever the window changes size (values in pixels).
    ///
    /// We resize our overlay here to take up the entire window.
    ///
    overlay_->Resize(width, height);
}

void Charcoal::OnFinishLoading(ultralight::View* caller,
    uint64_t frame_id,
    bool is_main_frame,
    const String& url) {
    ///
    /// This is called when a frame finishes loading on the page.
    ///
}



void Charcoal::OnChangeCursor(ultralight::View* caller,
    Cursor cursor) {
    ///
    /// This is called whenever the page requests to change the cursor.
    ///
    /// We update the main window's cursor here.
    ///
    window_->SetCursor(cursor);
}

void Charcoal::OnChangeTitle(ultralight::View* caller,
    const String& title) {
    ///
    /// This is called whenever the page requests to change the title.
    ///
    /// We update the main window's title here.
    ///
    window_->SetTitle(title.utf8().data());
}

#include <Ultralight/Ultralight.h>

using namespace ultralight;

inline std::string ToUTF8(const String& str) {
    String8 utf8 = str.utf8();
    return std::string(utf8.data(), utf8.length());
}

inline const char* Stringify(MessageSource source) {
    switch (source) {
    case kMessageSource_XML: return "XML";
    case kMessageSource_JS: return "JS";
    case kMessageSource_Network: return "Network";
    case kMessageSource_ConsoleAPI: return "ConsoleAPI";
    case kMessageSource_Storage: return "Storage";
    case kMessageSource_AppCache: return "AppCache";
    case kMessageSource_Rendering: return "Rendering";
    case kMessageSource_CSS: return "CSS";
    case kMessageSource_Security: return "Security";
    case kMessageSource_ContentBlocker: return "ContentBlocker";
    case kMessageSource_Other: return "Other";
    default: return "";
    }
}

inline const char* Stringify(MessageLevel level) {
    switch (level) {
    case kMessageLevel_Log: return "Log";
    case kMessageLevel_Warning: return "Warning";
    case kMessageLevel_Error: return "Error";
    case kMessageLevel_Debug: return "Debug";
    case kMessageLevel_Info: return "Info";
    default: return "";
    }
}

