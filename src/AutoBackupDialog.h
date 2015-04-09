#ifndef _AUTO_BACKUP_DIALOG_H_
#define _AUTO_BACKUP_DIALOG_H_

#include <memory>

using namespace std;

class AutoBackupDialog : public wxDialog, public ProgressUpdatable
{
public:
	static const int ID_RUN_TIMER = 0;

	AutoBackupDialog(wxWindow * parent);
	virtual void updateProgress(double value);
	inline virtual ~AutoBackupDialog() {};
	wxDECLARE_EVENT_TABLE();

private:
	void run(wxActivateEvent & event);

	wxGauge * progress = nullptr;
	unique_ptr <wxTimer> timer = nullptr;

	bool isRunning = false;
	int progressStages = 0, currentProgressStage = 0;
	float percentsPerStage = 0;
};

#endif // _AUTO_BACKUP_DIALOG_H_
