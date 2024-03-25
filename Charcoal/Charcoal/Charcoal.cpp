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
#define WINDOW_WIDTH  900
#define WINDOW_HEIGHT 600

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

    window_->SetTitle("Charcoal Outline");
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
                    // Your code here
                    
                    CoTaskMemFree(pszFilePath);
                }
                pItem->Release();
            }
        }
        pFileOpen->Release();
    }
    //add selected file to library
    std::string title = ebooks.add(pszFilePath);
    std::string added = ebooks.getStringData(title);
    MessageBoxA(NULL, added.c_str(), "Book Added", MB_OK);
    return;
}

void Charcoal::OnDOMReady(ultralight::View* caller,
    uint64_t frame_id,
    bool is_main_frame,
    const String& url) {
    RefPtr<JSContext> context = caller->LockJSContext();
    SetJSContext(context->ctx());
    JSObject global = JSGlobalObject();
    global["AddBook"] = BindJSCallback(&Charcoal::OpenFile);

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
