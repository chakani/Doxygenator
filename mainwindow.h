#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <algorithm>
#include <QFileInfo>

class cParam
{	public:
	QString name,
		type,			// RP_OPT *
		ref;			// RP_OPT
};

// also CPP files
class cDoxFunc
{	public:
	QString funcName, filePath,		// C:\Progeny\sandbox\PG011.CPP
		fileName,			// PG011.cpp
		type,	// return type (preferred)
		ref;
	std::vector <cParam> params;
	int lineNo,			// reference or prototype
		bodyStart;		// actual definition line
	void reset(void)
	{	// do not reset 'filePath', saved in extractXMLfuncs() and passed to memberdef()
		funcName = type = ref = "";
		params.clear();
		lineNo = -1;
	}
	bool operator <(cDoxFunc &oth)
	{	if(filePath.toUpper() < oth.filePath.toUpper())
			return true;
		else if(filePath.toUpper() > oth.filePath.toUpper())
			return false;
		else
			return bodyStart < oth.bodyStart;
	}
};

// for each function in a class or CPP file
class cDoxIndexEntry
{	public:
	QString name,			// for 'file', bare file name+suffix, no path
							// for class, just class name, no suffix
		filePath,			// complete dir + fname (for Files only, not classes)
		refid;			// file name in Dox Output directory, without '.xml' suffix
	bool operator <(cDoxIndexEntry &oth)
	{	return name.toUpper() < oth.name.toUpper();
	}
};

class cDoxIndex
{	public:
	std::vector <cDoxIndexEntry> fileNames;	// sorted by cDoxIndexEntry::name
	void dump(const QString &indexFileName, const QString fname);
};

class cDoxEnviron
{	public:
	QString cppFileWithPath,			// includes path "C:\Progeny\sandbox\pg087.cpp"
		cppFileNameOnly,			// "PG087.CPP"
		baseDirectory,			// includes trailing backslash
		DoxOutputDirectory,		// C:/Doxygen/Charting Companion/
		xmlCPPfile,				// xml file corresponding to CPP
		configFilePath, configFileDir;
	std::vector <cDoxFunc> funcs, classes;	// sort classes by filePath & line no.
	cDoxIndex doxClassIndex, doxFileIndex;

	void dump(void);
	void dump(std::vector <cDoxFunc> &, QFileInfo &outputFileInfo);
	void reset(void)
	{	funcs.clear();
		cppFileWithPath.clear(), cppFileNameOnly.clear();
	}
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();
	bool getDoxConfig(cDoxEnviron &doxFile);

private slots:
	void on_commandLinkButton_clicked();
	bool initialize(cDoxEnviron &doxFile);
private:
	Ui::MainWindow *ui;
};
void dump(std::vector <cDoxFunc> &doxFuncs, QString cppFilePath, QString fileType);
bool insertDoxCommands(cDoxEnviron &doxEnviron, QString &message);
void getDoxConfig(cDoxEnviron &doxEnviron);
bool loadIndex(MainWindow *window, const QString indexFileName, cDoxIndex &doxClassIndex, cDoxIndex &doxFileIndex);
bool extractCPPlocationFromXML(QString &filesCPPfileName, cDoxEnviron &doxEnviron, cDoxIndexEntry &xit);

#endif // MAINWINDOW_H
