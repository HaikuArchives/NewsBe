// all includes here
#include "YourApp.h"

//------------
// main()
//------------

int main()
{
	YourApp *app;
	app_info appInfo; 
	BEntry *beAppEntry;   	
	BEntry *beAppParent;
	BPath  *bpAppPath;
	FILE *fPoll;
	char *sPollPath;
	char *sAppPath;
	dev_t dtAppDevice;
	sAppPath = (char *)malloc(B_PATH_NAME_LENGTH);
	sPollPath = (char *)malloc(B_PATH_NAME_LENGTH);
	beAppEntry = new BEntry; 
	beAppParent = new BEntry; 
	bpAppPath = new BPath;
	DIR *dAppDir;
	int rc;
	
	app = new YourApp();
	create_directory("/boot/home/news", 0777);
 	if ( be_app->GetAppInfo(&appInfo) == B_OK ) 
  	{
   		beAppEntry->SetTo(&(appInfo.ref));
  		beAppEntry->GetParent(beAppParent);
  		beAppParent->GetPath(bpAppPath);
   		sAppPath = strcpy(sAppPath, bpAppPath->Path());
   		sPollPath = strcpy(sPollPath, sAppPath);
   		strcat(sAppPath,"/lastpoll");
   	}
	if(NULL == (fPoll = fopen(sAppPath,"r")))
	{
		dtAppDevice = dev_for_path(sAppPath);
		dAppDir = fs_open_index_dir(dtAppDevice);
		rc = fs_create_index(dtAppDevice, "NEWS:newsgroup",B_STRING_TYPE,0);
		rc = fs_create_index(dtAppDevice, "NEWS:date",B_STRING_TYPE,0);
		rc = fs_create_index(dtAppDevice, "NEWS:poll", B_INT32_TYPE,0);		
		rc = fs_create_index(dtAppDevice, "NEWS:server",B_STRING_TYPE,0);
		rc = fs_create_index(dtAppDevice, "NEWS:state",B_STRING_TYPE,0);
		rc = fs_create_index(dtAppDevice, "NEWS:subject",B_STRING_TYPE,0);
	}
	fclose(fPoll);	

	app->Run();

	delete app;
}

//-------------
// YourApp()
//-------------

YourApp::YourApp()	
	: BApplication("application/x-vnd.VargolSoft-NewsBe")
{

// instantiate a new window, adjust for sceen size ???
	win = new NewsBeMainWindow(BRect(20, 30, 760, 556), sNewsBeVersion, 
		B_TITLED_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_ASYNCHRONOUS_CONTROLS, 
		B_CURRENT_WORKSPACE);

// make window visible
	win->Show();
}