/** \page page2 Documentation links
	\li \c Classes https://doc.qt.io/qt-5.15/qtcore-module.html
	\li \c QString https://doc.qt.io/qt-5.15/qstring.html
	\li \c QFile https://doc.qt.io/qt-5.15/qfile.html
	\li \c QFileInfo https://doc.qt.io/qt-5.15/qfileinfo.html
	\li \c QDomDocument https://doc.qt.io/qt-5.15/qdomdocument.html
	\li \c QFileOpen dialog https://doc.qt.io/qt-6/qfiledialog.html
	\li \c QProgressDialog https://doc.qt.io/qt-5/qprogressdialog.html
	\li \c QStatusBar https://doc.qt.io/qt-6/qstatusbar.html
	\li \c QMessageBox https://doc.qt.io/qt-6/qmessagebox.html

	\li \c Configuration file https://www.doxygen.nl/manual/config.html
	\li \c Doxygen commands https://www.doxygen.nl/manual/commands.html

	Non-breaking space xA0 ' '\n

"C:\Program Files\doxygen\bin\doxygen.exe" "C:\Progeny\CC_Qt\ChartingCompanion_Doxygen (file subset).cfg"\n
"C:\Program Files\doxygen\bin\doxygen.exe" "C:\Progeny\CC_Qt\ChartingCompanion_Doxygen.cfg"\n
"C:\Program Files\doxygen\bin\doxygen.exe" "C:\Progeny\CC_Qt\ChartingCompanion_Doxygen.cfg" 1>>C:\temp\doxygen.log 2>>C:\temp\doxygen_errors.log\n
\verbatim
VERY IMPORTANT: in Doxygen Config file, must have:\n
FULL_PATH_NAMES        = YES
STRIP_FROM_PATH        = NO
STRIP_FROM_INC_PATH    = NO
EXCLUDE                = *_doxbackup.cpp
XML_PROGRAMLISTING     = NO
INCLUDE_PATH = (specify all directories with header files)
PREDEFINED             = WIN64 (Copy from Project file)
\endverbatim
	*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <C:\Qt\5.13.0\msvc2017_64\include\QtXml\qdom.h>
#include <qsettings.h>
#include <qstandardpaths.h>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QProgressDialog>
#include <QDateTime>

using namespace std;

bool bDumpClassesFuncs = false;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}

// compounddef.sectiondef.memberdef
bool memberdef(cDoxEnviron &doxEnviron, std::vector <cDoxFunc> &list, const QDomNode &memberdefNode, bool bFullDetailList)
{	cDoxFunc wkFunc;

	QDomNamedNodeMap attribs = memberdefNode.attributes();
	if(attribs.contains("kind"))
	{	QDomNode kind = attribs.namedItem("kind");
		if(bFullDetailList)
			qDebug() << "\t\t\t\t\t" << kind.nodeName() << kind.nodeValue();
		if(kind.nodeValue() != "function")
			return false;
		// we got a live one
		wkFunc.reset();
		// compounddef.sectiondef.memberdef.location
		QDomNode locationNode = memberdefNode.namedItem("location");
		if(locationNode.isNull())
			return false;

		// compounddef.sectiondef.memberdef.location.bodyfile
		QDomNamedNodeMap locAttribs = locationNode.attributes();
		if(locAttribs.contains("bodyfile"))
		{	QDomNode bodyfile = locAttribs.namedItem("bodyfile");
			//qDebug() << "\t\t\t\t\t" << bodyfile.nodeName() << bodyfile.nodeValue();
			wkFunc.filePath = bodyfile.nodeValue();
			QFileInfo bodyFileInfo(wkFunc.filePath);
			wkFunc.fileName = bodyFileInfo.fileName();
			// some files are missing path, ex. when they are in the same directory as the Doxygen config file
			if(bodyFileInfo.isRelative())
			{	QFileInfo completePath(doxEnviron.configFileDir, wkFunc.fileName);
				wkFunc.filePath = completePath.absoluteFilePath();
			}
		}
		else
			return false;	// just a prototype

		// compounddef.sectiondef.memberdef.name
		QDomElement nameElem = memberdefNode.namedItem("name").toElement();
		if(bFullDetailList)
			qDebug() << "\t\t\t\t\t" << nameElem.nodeName() << nameElem.text();
		if(!nameElem.isNull())
			wkFunc.funcName = nameElem.text();

		// compounddef.sectiondef.memberdef.type
		QDomNode memberTypeNode = memberdefNode.namedItem("type");
		if(!memberTypeNode.isNull())
		{	QDomElement memberTypeElem = memberTypeNode.toElement();
			wkFunc.type = memberTypeElem.text();
			QDomNode memberTypeRef = memberTypeNode.namedItem("ref");
			// compounddef.sectiondef.memberdef.type.ref
			if(!memberTypeRef.isNull())
			{	wkFunc.ref = memberTypeRef.toElement().text();
			}
		}

		// compounddef.sectiondef.memberdef.location.line
		if(locAttribs.contains("line"))
		{	QDomNode bodyfile = locAttribs.namedItem("line");
			//qDebug() << "\t\t\t\t\t" << bodyfile.nodeName() << bodyfile.nodeValue();
			wkFunc.lineNo = bodyfile.nodeValue().toInt();
		}

		// compounddef.sectiondef.memberdef.location.bodystart
		if(locAttribs.contains("bodystart"))
		{	QDomNode bodyfile = locAttribs.namedItem("bodystart");
			//qDebug() << "\t\t\t\t\t" << bodyfile.nodeName() << bodyfile.nodeValue();
			wkFunc.bodyStart = bodyfile.nodeValue().toInt();
		}

		// compounddef.sectiondef.memberdef.param
		for(QDomNode paramNode = memberdefNode.namedItem("param"); !paramNode.isNull();
		paramNode = paramNode.nextSibling())
		{	cParam wkParam;
			if(paramNode.nodeName() != "param")
				continue;
			// compounddef.sectiondef.memberdef.param.declname
			QDomElement paramNameElem = paramNode.namedItem("declname").toElement();
			//qDebug() << "\t\t\t\t\t" << paramNode.nodeName() << paramNameElem.text();
			if(!paramNameElem.isNull())
				wkParam.name = paramNameElem.text();
			// compounddef.sectiondef.memberdef.param.type
			QDomNode paramTypeNode = paramNode.namedItem("type");
			if(!paramTypeNode.isNull())
			{	QDomElement paramTypeElem = paramTypeNode.toElement();
				wkParam.type = paramTypeElem.text();
				QDomNode paramTypeRef = paramTypeNode.namedItem("ref");
				// compounddef.sectiondef.memberdef.param.type.ref
				if(!paramTypeRef.isNull())
				{	wkParam.ref = paramTypeRef.toElement().text();
				}
			}
			wkFunc.params.push_back(wkParam);
		}
		// save it
		list.push_back(wkFunc);
	}
	return true;
}

/*
compounddef
	sectiondef kind="func"
		memberdef kind="function"
			<name>pg_moveto_clip</name>
			<param>
				<declname>hdc</declname>
			<location file="pg087.cpp" line="71" column="7" declfile="pg087.cpp" declline="71" declcolumn="7"/> (prototype)

			<location file="pg087.cpp" line="153" column="6" bodyfile="pg087.cpp" bodystart="153" bodyend="319"/> (definition)

Does NOT sort 'doxEnviron': must be sorted by caller; this allows for compound loading
*/
bool extractXMLfuncs(const QString xmlFileName, cDoxEnviron &doxEnviron, std::vector <cDoxFunc> &list)
{	int i, j, k, m;
	QString errorStr;
	int errorLine, errorColumn;
	bool bFullDetailList = false;
	cDoxFunc wkFunc;

	QFile xmlFile(xmlFileName);
	if (!xmlFile.open(QIODevice::ReadOnly))
	{	QString err = "Error unable to open Doxygen XML file " + xmlFileName;
		qDebug() << err;
		QMessageBox(QMessageBox::Critical, "Error", err).exec();
		return false;
	}

	QDomDocument doc("DOXfile");
	if (!doc.setContent(&xmlFile, false, &errorStr, &errorLine, &errorColumn))
	{	QString err("Error parsing " + xmlFileName + " ");
		err += QString("%1 %2 %2").arg(errorStr).arg(errorLine).arg(errorColumn);
		qDebug() << err;
		QMessageBox(QMessageBox::Critical, "Error", err).exec();
		return false;
	}
	doxEnviron.xmlCPPfile = xmlFileName;
	QDomNodeList topList = doc.elementsByTagName("doxygen");
	for(i = 0; i < topList.size(); i++)
	{	if(bFullDetailList)
			qDebug() << topList.at(i).nodeName() << topList.at(i).nodeValue();
		QDomNodeList compoundList = topList.at(i).childNodes();
		for(j = 0; j < compoundList.size(); j++)
		{	if(bFullDetailList)
				qDebug() << "\t" << compoundList.at(j).nodeName() << compoundList.at(j).nodeValue();
			if(compoundList.at(j).nodeName() != "compounddef")
				continue;
			// compounddef
			// compounddef.compoundname
			/* not necessary, full path name already known
			QDomNode compoundnameNode = compoundList.at(j).namedItem("compoundname");
			if(!compoundnameNode.isNull())
			{	doxEnviron.cppFileNameOnly = compoundnameNode.toElement().text();
				QFileInfo cppFilePath(doxEnviron.configFileDir, doxEnviron.cppFileNameOnly);
				doxEnviron.cppFileWithPath = cppFilePath.absoluteFilePath();
			}
			*/

			QDomNodeList sectionList = compoundList.at(j).childNodes();
			for(k = 0; k < sectionList.size(); k++)
			{	if(bFullDetailList)
					qDebug() << "\t\t" << sectionList.at(k).nodeName() << sectionList.at(k).nodeValue();
				if(sectionList.at(k).nodeName() != "sectiondef")
					continue;
				// compounddef.sectiondef
				QDomNamedNodeMap attribs = sectionList.at(k).attributes();
				if(attribs.contains("kind"))
				{	QDomNode kind = attribs.namedItem("kind");
					if(bFullDetailList)
						qDebug() << "\t\t\t" << kind.nodeName() << kind.nodeValue();
					if(kind.nodeValue() != "func" && kind.nodeValue() != "public-func")
						continue;
					QDomNodeList memberList = sectionList.at(k).childNodes();
					for(m = 0; m < memberList.size(); m++)
					{	if(bFullDetailList)
							qDebug() << "\t\t\t\t" << memberList.at(m).nodeName() << memberList.at(m).nodeValue();
						if(memberList.at(m).nodeName() != "memberdef")
							continue;
						// compounddef.sectiondef.memberdef
						memberdef(doxEnviron, list, memberList.at(m), bFullDetailList);
					}
				}

			}
		}
	}
	xmlFile.close();
	return true;
}

bool fileComp(cDoxIndexEntry elem1, cDoxIndexEntry elem2)
{	return elem1.name.toUpper() < elem2.name.toUpper();
}

// get list of classes, source files
bool MainWindow::initialize(cDoxEnviron &doxEnviron)
{
	// get Doxygen config file
	statusBar()->showMessage("Loading Doxygen config file");
	if(!getDoxConfig(doxEnviron))
	{	return false;
	}
	// Read Dox index file, load list of CPP file names & classes
	statusBar()->showMessage("Loading Doxygen index file");
	statusBar()->update();
	QString indexFileName = doxEnviron.DoxOutputDirectory + "index.xml";
	if(!loadIndex(this, indexFileName,doxEnviron.doxClassIndex, doxEnviron.doxFileIndex))		// displays error message
		return false;

	// load all classes
	statusBar()->showMessage("Loading Classes");
	QProgressDialog *progressBar = new QProgressDialog("Loading classes...", "Cancel", 0, doxEnviron.doxClassIndex.fileNames.size(), this);
	progressBar->setWindowModality(Qt::WindowModal);
	vector <cDoxIndexEntry>::iterator xit;
	for(xit = doxEnviron.doxClassIndex.fileNames.begin(); xit != doxEnviron.doxClassIndex.fileNames.end(); xit++)
	{	QString classFileName = doxEnviron.DoxOutputDirectory + xit->refid + ".xml";
		QFile classFile(classFileName);
		progressBar->setValue(xit - doxEnviron.doxClassIndex.fileNames.begin());
		if (progressBar->wasCanceled())
			break;
		if(classFile.exists())
		{	extractXMLfuncs(classFileName, doxEnviron, doxEnviron.classes);
		}
	}
	std::sort(doxEnviron.classes.begin(), doxEnviron.classes.end());	// sort classes by filePath & line no.
	progressBar->close();
	delete progressBar;
	// dump to debugging file
	if(bDumpClassesFuncs)
		dump(doxEnviron.classes, doxEnviron.configFilePath, "classes");

	// load all files to get their absolute location
	statusBar()->showMessage("Loading Files");
	progressBar = new QProgressDialog("Loading CPP's XML files...", "Cancel", 0, doxEnviron.doxFileIndex.fileNames.size(), this);
	progressBar->setWindowModality(Qt::WindowModal);
	for(xit = doxEnviron.doxFileIndex.fileNames.begin(); xit != doxEnviron.doxFileIndex.fileNames.end(); xit++)
	{	QString filesFileName = doxEnviron.DoxOutputDirectory + xit->refid + ".xml";
		QFile filesFile(filesFileName);
		progressBar->setValue(xit - doxEnviron.doxFileIndex.fileNames.begin());
		if (progressBar->wasCanceled())
			break;
		if(filesFile.exists())
		{	extractCPPlocationFromXML(filesFileName, doxEnviron, *xit);
		}
	}
	if(bDumpClassesFuncs)
		doxEnviron.doxFileIndex.dump(indexFileName, "list of Files.txt");

	progressBar->setValue(doxEnviron.doxFileIndex.fileNames.size());
	statusBar()->clearMessage();
	return true;
}

// main entry point
void MainWindow::on_commandLinkButton_clicked()
{	static cDoxEnviron doxEnviron, classes;
	QSettings *settings;
	QString dirName, err, message;
	QStringList qstrDir = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
	static bool bInitialized = false;

	statusBar()->clearMessage();
	if(!bInitialized)
	{	if(!initialize(doxEnviron))
			return;
		bInitialized = true;
	}

	// QSettings file
	QString m_path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) ;
	QString m_filename = "Doxygen_config.ini";
	// Stores integers as strings
	settings = new QSettings(m_path + "/"+ m_filename, QSettings::IniFormat) ;

	settings->beginGroup("DOXYGEN");
	// Load save file location, else default to documents
	if (settings->value("file").isValid())
		dirName = settings->value("file").toString();
	else
		dirName = qstrDir[0];

	// Get path(s) of CPP from user
	QStringList fileList = QFileDialog::getOpenFileNames(this, "Process CPP file(s)", dirName, "CPP files (*.cpp)");
	if(fileList.size() > 0)
		settings->setValue("file", fileList[0]);
	settings->endGroup();

	// process the lot
	for(int f = 0; f < fileList.size(); f++)
	{	cDoxIndexEntry wkFile;

		doxEnviron.reset();	// make re-entrant
		doxEnviron.cppFileWithPath = fileList[f];
		QFileInfo cppFileInfo(fileList[f]);
		doxEnviron.cppFileNameOnly = cppFileInfo.fileName();

		// find corresponding XML file
		wkFile.name = cppFileInfo.fileName();	// includes suffix
		// result.first is first element that is not less than value,
		// result.second  is first element greater
		// Initially, result.first == result.second after being declared and before being used
		pair< vector< cDoxIndexEntry >::iterator, vector< cDoxIndexEntry >::iterator > result;
		result = std::equal_range(doxEnviron.doxFileIndex.fileNames.begin(), doxEnviron.doxFileIndex.fileNames.end(),
			wkFile, fileComp);		// compares cDoxIndexEntry::name only
		if(result.first != result.second && result.first->name.toUpper() == wkFile.name.toUpper())
		{	QFileInfo xmlPath(doxEnviron.DoxOutputDirectory, result.first->refid + ".xml");

			statusBar()->showMessage("Processing " + result.first->name);
			//statusBar()->update();
			extractXMLfuncs(xmlPath.absoluteFilePath(), doxEnviron, doxEnviron.funcs);
			std::sort(doxEnviron.funcs.begin(), doxEnviron.funcs.end());
			// dump to debugging file
			if(bDumpClassesFuncs)
				dump(doxEnviron.funcs, doxEnviron.cppFileWithPath, "functions");
			insertDoxCommands(doxEnviron, message);
		}
		else
		{	QString err = "Error unable to find CPP " + wkFile.name + " in doxFileIndex" ;
			qDebug() << err;
			if(QMessageBox(QMessageBox::Critical, "Error", err, QMessageBox::Ok | QMessageBox::Cancel).exec() == 0)
				break;
		}
	}
	if(fileList.size() > 0)
		QMessageBox(QMessageBox::Information, "Success", message).exec();
	statusBar()->showMessage("Completed");
}

QString alphaType(const QString type)
{	QString bare = type;

	bare = bare.remove("const");
	bare = bare.remove("&");
	bare = bare.remove(" ");
	bare = bare.remove("*");
	if(bare.contains("cGDIp"))
		return "Graphical Device Context";
	else if(bare.contains("HDC"))
		return "Handle to Device Context (Windows)";
	else if(bare.contains("HWND"))
		return "Handle to a window (Windows)";
	else if(bare.contains("RP_OPT"))
		return "Chart options";
	else if(bare == "AN" || bare == "cAN")
		return "Ancestor node";
	else if(bare == "DN" || bare == "cDN")
		return "Descendant node";
	else if(bare == "PE" || bare == "cPE")
		return "Person";
	else if(bare == "PL" || bare == "cPL")
		return "Place";
	else if(bare == "EV" || bare == "cEV")
		return "Event";
	else if(bare == "FO" || bare == "cFO")
		return "Source Citation";
	else if(bare == "RECT")
		return "Rectangle";
	else if(bare == "EVN")
		return "Linked list of events";
	else
		return "";

}

bool funcComp(cDoxFunc elem1, cDoxFunc elem2)
{	return elem1.filePath.toUpper() < elem2.filePath.toUpper();
}


/** \brief Copy Doxygen parameters into source file
	* Comment
	* \param outputFile Updated C++  code with comments
	* \param funcit one "parameter packet"
	* \return nothing
	* \date
	* \deprecated
	* \details
	* \note
	* \warning
	* \par [(paragraph title)] { paragraph }
	* \remark
	* \todo
	* \a <word> italic
	* \b bold
	* \c Fixed-pitch font
	* \code{.unparsed}
		Show this as-is please
		\endcode
	* \li \c keyword Description  Lists
	* \n New line
	* \verbatim
	*	\endverbatim
	* \bug
	*/
void injectDoxComments(QFile &outputFile, cDoxFunc &funcit)
{	vector <cParam>::iterator parmit;
	QString buf;

	outputFile.write("/** \\brief \n");
	for(parmit = funcit.params.begin(); parmit != funcit.params.end(); parmit++)
	{	if(parmit->type == "void" && parmit->name.size() == 0)
			continue;
		buf = "\t* \\param " + parmit->name + " (" + parmit->type + ") " + alphaType(parmit->type) + " \n";
		outputFile.write(buf.toLatin1());
	}
	if(funcit.type != "void")
	{	buf = "\t* \\return " + funcit.type + " \n";
		outputFile.write(buf.toLatin1());
	}
	outputFile.write("\t* \\details \n");
	outputFile.write("\t*/\n");
}

bool insertDoxCommands(cDoxEnviron &doxEnviron, QString &message)
{	char buf[2048];
	// backup old file
	QFile fileCPP(doxEnviron.cppFileWithPath);
	vector <cDoxFunc>::iterator funcit, classit;
	QString err;
	int totInjected = 0;

	// check file exists
	if(!fileCPP.exists())
	{	err = "Error CPP file not found " + doxEnviron.cppFileWithPath;
		qDebug() << err;
		QMessageBox(QMessageBox::Critical, "Error", err).exec();
		return false;
	}

	// check XML file more recent
	QFileInfo xmlFileInfo(doxEnviron.xmlCPPfile);
	QFileInfo fileInfo = QFileInfo(fileCPP);
	if(fileInfo.lastModified() > xmlFileInfo.lastModified())
	{	err	= "File " + doxEnviron.xmlCPPfile + " is older than " + doxEnviron.cppFileWithPath
			+ "\nMay not be compatible";
		qDebug() << err;
		QMessageBox(QMessageBox::Critical, "Error", err).exec();
		return false;
	}

	// make backup file
	QString fileName = fileInfo.completeBaseName();
	QFileInfo backupFileInfo(fileInfo.path(), fileName + "_doxbackup" + "." + fileInfo.suffix());
	QFile backupFile(backupFileInfo.filePath());
	if(backupFile.exists())
		backupFile.remove();
	if(!fileCPP.copy(backupFileInfo.filePath()))
	{	err	= "Error cannot backup CPP file " + doxEnviron.cppFileWithPath + backupFileInfo.filePath();
		qDebug() << err;
		QMessageBox(QMessageBox::Critical, "Error", err).exec();
		return false;
	}

	// insert Doxygen function comments
	// open input file
	if(!fileCPP.open(QIODevice::ReadOnly))
	{	err = "Error CPP file cannot open input " + doxEnviron.cppFileWithPath;
		qDebug() << err;
		QMessageBox(QMessageBox::Critical, "Error", err).exec();
		return false;
	}

	// open output file
	QFileInfo outputFileInfo(fileInfo.path(), fileName + "_doxoutput" + "." + fileInfo.suffix());
	QFile outputFile(outputFileInfo.filePath());
	if(!outputFile.open(QIODevice::WriteOnly))
	{	err = "Error CPP file cannot open output " + outputFileInfo.filePath();
		qDebug() << err;
		QMessageBox(QMessageBox::Critical, "Error", err).exec();
		return false;
	}

	// three-way merge
	cDoxFunc wkFunc;
	int line = 0, mostRecentComment = -1;
	if(bDumpClassesFuncs)
	{	if(doxEnviron.funcs.begin() == doxEnviron.funcs.end())
			qDebug() << "No functions for " << doxEnviron.cppFileWithPath;
		wkFunc.filePath =doxEnviron.cppFileWithPath;
	}

	// search for sub-range of this CPP file in classes
	pair< vector< cDoxFunc >::iterator, vector< cDoxFunc >::iterator > resultClasses;
	resultClasses = std::equal_range(doxEnviron.classes.begin(), doxEnviron.classes.end(), wkFunc,
		funcComp);	// compares file paths
	if(bDumpClassesFuncs)
	{	if(resultClasses.first == resultClasses.second)
			qDebug() << "No class matches for " << wkFunc.filePath;
		else
			qDebug() << "Classes range " << resultClasses.first->bodyStart << " to " << (resultClasses.second - 1)->bodyStart;
	}
	int totClassContrib = resultClasses.second - resultClasses.first;
	classit = resultClasses.first;

	// search for sub-range of this CPP file in functions
	pair< vector< cDoxFunc >::iterator, vector< cDoxFunc >::iterator > resultFuncs;
	resultFuncs = std::equal_range(doxEnviron.funcs.begin(), doxEnviron.funcs.end(), wkFunc,
		funcComp);	// compares file paths
	if(bDumpClassesFuncs)
	{	if(resultFuncs.first == resultFuncs.second)
			qDebug() << "No function matches for" << wkFunc.filePath;
		else
			qDebug() << "Functions range " << resultFuncs.first->bodyStart << " to " << (resultFuncs.second - 1)->bodyStart;
	}
	int totFuncContrib = resultFuncs.second - resultFuncs.first;
	funcit = resultFuncs.first;

	while(fileCPP.readLine(buf, sizeof(buf)) > 0)
	{	++line;
		// capture most recent comment
		if(strncmp(buf, " */", 3) == 0 || strncmp(buf, "\t*/", 3) == 0)
			mostRecentComment = line;

		// catch up
		while(classit != resultClasses.second && classit->bodyStart < line)
			classit++;
		while(funcit != resultFuncs.second && funcit->bodyStart < line)
			funcit++;
		if(funcit != resultFuncs.second && line == funcit->bodyStart)
		{	if(bDumpClassesFuncs)
				qDebug() << funcit->funcName << "matches at line " << line;
			if(mostRecentComment == -1 || line - mostRecentComment > 3)		// does this func already have a comment block?
			{	injectDoxComments(outputFile, *funcit);				// if not
				totInjected++;
			}
			funcit++;
		}
		else if(classit != resultClasses.second && line == classit->bodyStart)
		{	if(mostRecentComment == -1 || line - mostRecentComment > 3)
			{	injectDoxComments(outputFile, *classit);
				totInjected++;
			}
			classit++;
		}
		else if(funcit != resultFuncs.second && line > funcit->bodyStart)
		{	funcit++;
			err = "Error function not found " + funcit->funcName + QString("%1").arg(funcit->lineNo);
			qDebug() << err;
			QMessageBox(QMessageBox::Critical, "Error", err).exec();
		}
		outputFile.write(buf);
	}
	fileCPP.close();
	outputFile.close();

	/* change names*/
	if(!fileCPP.remove())
	{	err = "Unable to delete old " + doxEnviron.cppFileWithPath;
		QMessageBox(QMessageBox::Critical, "Error", err).exec();
		return false;
	}

	if(!outputFile.rename(doxEnviron.cppFileWithPath))
	{	err = "Unable to rename " + outputFileInfo.filePath() + " to " + doxEnviron.cppFileWithPath;
		QMessageBox(QMessageBox::Critical, "Error", err).exec();
		return false;
	}


	err = "Successfully converted " + doxEnviron.cppFileWithPath + "\n"
		+ QString("%1 total functions, %2 commented\n").arg(totFuncContrib + totClassContrib).arg(totInjected);
	message += err;
	//QMessageBox(QMessageBox::Information, "Success", err).exec();
	qDebug() << err;
	return true;
}

bool MainWindow::getDoxConfig(cDoxEnviron &doxEnviron)
{	QSettings *settings;
	QString dirName;
	char buf[1024];
	bool bGotInput = false, bGotOutput = false;

	// QSettings file
	QString m_path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) ;
	QString m_filename = "Doxygen_config.ini";
	// Stores integers as strings
	settings = new QSettings(m_path + "/"+ m_filename, QSettings::IniFormat) ;

	settings->beginGroup("DOXYGEN");
	// Load save file location, else default to documents
	if (settings->value("config").isValid())
		dirName = settings->value("config").toString();
	else
		dirName = "C:\\Progeny\\CC_Qt";

	// Get path of xml from user
	// QFileDialog::ExistingFiles
	doxEnviron.configFilePath = QFileDialog::getOpenFileName(this, "Load Doxygen Configuration file", dirName, "Config files (*.*)");
	if(doxEnviron.configFilePath.isNull())
		return false;

	settings->setValue("config", doxEnviron.configFilePath);
	settings->value("config").isValid();
	settings->endGroup();

	QFileInfo configFileInfo(doxEnviron.configFilePath);
	doxEnviron.configFileDir = configFileInfo.path();

	QFile configFile(doxEnviron.configFilePath);

	if(configFile.open(QIODevice::ReadOnly))
	{	while(configFile.readLine(buf, sizeof(buf)) > 0)
		{	if(strncmp(buf, "INPUT ", 6) == 0 || strncmp(buf, "OUTPUT_DIRECTORY", 16) == 0)
			{	QString inputLine(buf);
				qsizetype equalOff = inputLine.indexOf("=");
				QString mid = inputLine.mid(equalOff + 1);
				mid = mid.trimmed();	// removes leading + trailing blanks, \n
				mid = mid.remove("\"");		// trailing backslash
				mid += "/";
				if(strncmp(buf, "INPUT", 5) == 0)
				{	doxEnviron.baseDirectory = mid;	// not needed, could be detailed list of individual files
					bGotInput = true;
				}
				else if(strncmp(buf, "OUTPUT_DIRECTORY", 16) == 0)
				{	doxEnviron.DoxOutputDirectory = mid + "xml/";
					bGotOutput = true;
				}
			}
			if(bGotOutput)
				break;
		}
		configFile.close();
	}
	else
	{	QString err = "Error unable to load Doxygen config file " + doxEnviron.cppFileWithPath;
		qDebug() << err;
		QMessageBox(QMessageBox::Critical, "Error", err).exec();
		return false;
	}

	return true;
}

void cDoxEnviron::dump(void)
{	std::vector <cDoxFunc>::iterator funcit;
	std::vector <cParam>::iterator parmit;
	qDebug() << "Functions";
	for(funcit = funcs.begin(); funcit != funcs.end(); funcit++)
	{	qDebug() << funcit->fileName << funcit->filePath << funcit->funcName << funcit->bodyStart;
		for(parmit = funcit->params.begin(); parmit != funcit->params.end(); parmit++)
			qDebug() << "\t" << parmit->name;
	}
	qDebug() << "Classes";
	for(funcit = classes.begin(); funcit != classes.end(); funcit++)
	{	qDebug() << funcit->filePath << funcit->bodyStart << funcit->fileName << funcit->funcName;
		for(parmit = funcit->params.begin(); parmit != funcit->params.end(); parmit++)
			qDebug() << "\t" << parmit->name;
	}
}

void dump(std::vector <cDoxFunc> &doxFuncs, QString cppFilePath, QString fileType)
{	QFileInfo cppFileInfo(cppFilePath);

	QFileInfo outputFileInfo(cppFileInfo.path(), cppFileInfo.completeBaseName() + "_" + fileType + ".txt");

	QFile outputFile(outputFileInfo.filePath());
	if(outputFile.open(QIODevice::WriteOnly))
	{	std::vector <cDoxFunc>::iterator fit;
		for(fit = doxFuncs.begin(); fit != doxFuncs.end(); fit++)
		{	QString buf = fit->filePath + " " + fit->funcName + " " + QString("%1").arg(fit->bodyStart) + "\n";
			outputFile.write(buf.toLatin1());
		}
		outputFile.close();
	}
}

/* Load the Doxygen .index.xml file, containing list of all classes and CPP files
Classes:
<doxygenindex ...>
	<compound refid="class_ancestor_book_wizard" kind="class">
		<name>AncestorBookWizard</name>
		<member refid="class_ancestor_book_wizard_1a2bf3e9e9dddc0ba3c5e569300d3c616a" kind="function">
			<name>AncestorBookWizard</name>
		</member>
		<member refid="class_ancestor_book_wizard_1a7a6ee176f53511f1b35469cb3b0b9002" kind="function">
			<name>eventFilter</name>
		</member>
	</compound>

Files:
	<compound refid="ancestor_book_dialog_8cpp" kind="file">
		<name>ancestorBookDialog.cpp</name>
	</compound>
*/
bool loadIndex(MainWindow *window, const QString indexFileName, cDoxIndex &doxClassIndex, cDoxIndex &doxFileIndex)
{	QString errorStr;
	int errorLine, errorColumn;
	bool bFullDetailList = false;
	int i, j;
	cDoxIndexEntry wkIndexEntry;
	enum KIND {CLASS_KIND, FILE_KIND};
	KIND compoundKind;

	QFile xmlFile(indexFileName);
	if (!xmlFile.open(QIODevice::ReadOnly))
	{	errorStr = "Error unable to open Index file " + indexFileName;
		qDebug() << errorStr;
		QMessageBox(QMessageBox::Critical, "Error", errorStr).exec();
		return false;
	}

	QDomDocument doc("DOXindex");
	if (!doc.setContent(&xmlFile, false, &errorStr, &errorLine, &errorColumn))
	{	qDebug() << "Error" << errorStr << errorLine << errorColumn;
		QMessageBox(QMessageBox::Critical, "Error", errorStr).exec();
		return false;
	}
	// 'topList' only contains one element, named 'doxygenindex'
	QDomNodeList topList = doc.elementsByTagName("doxygenindex");
	for(i = 0; i < topList.size(); i++)
	{	if(bFullDetailList)
			qDebug() << topList.at(i).nodeName() << topList.at(i).nodeValue();
		QDomNodeList compoundList = topList.at(i).childNodes();
		QProgressDialog progressBar("Loading Doxygen Index...", "Cancel", 0, compoundList.size(), window);
		progressBar.setWindowModality(Qt::WindowModal);
		for(j = 0; j < compoundList.size(); j++)
		{	progressBar.setValue(j);

			if(bFullDetailList)
				qDebug() << "\t" << compoundList.at(j).nodeName() << compoundList.at(j).nodeValue();
			if(compoundList.at(j).nodeName() != "compound")
				continue;
			// compound
			QDomNamedNodeMap attribs = compoundList.at(j).attributes();
			if(attribs.contains("kind"))
			{	QDomNode kind = attribs.namedItem("kind");
				if(bFullDetailList)
					qDebug() << "\t\t\t" << kind.nodeName() << kind.nodeValue();
				if(kind.nodeValue() == "class")
					compoundKind = CLASS_KIND;
				else if(kind.nodeValue() == "file")
					compoundKind = FILE_KIND;
				else
					continue;
			}
			else
				continue;

			if(attribs.contains("refid"))
			{	QDomNode refid = attribs.namedItem("refid");
				if(bFullDetailList)
					qDebug() << "\t\t\t" << refid.nodeName() << refid.nodeValue();
				wkIndexEntry.refid = refid.nodeValue();
			}
			else
				continue;

			// compound.name
			QDomNode nameNode = compoundList.at(j).namedItem("name");
			if(nameNode.isNull())
				continue;
			else
			{	wkIndexEntry.name = nameNode.toElement().text();
			}

			if(compoundKind == CLASS_KIND)
				doxClassIndex.fileNames.push_back(wkIndexEntry);
			else if(compoundKind == FILE_KIND)
				doxFileIndex.fileNames.push_back(wkIndexEntry);
		}
		progressBar.setValue(progressBar.maximum());
	}
	std::sort(doxClassIndex.fileNames.begin(), doxClassIndex.fileNames.end());	// sorted by cDoxIndexEntry::name
	std::sort(doxFileIndex.fileNames.begin(), doxFileIndex.fileNames.end());	// (file name, no path)

	//doxClassIndex.dump();
	return true;
}

void cDoxIndex::dump(const QString &indexFileName, const QString fname)
{	std::vector <cDoxIndexEntry>::iterator iit;

	QFileInfo indexFileInfo(indexFileName);
	QFileInfo outputFileInfo(indexFileInfo.path(), fname);
	QFile outputFile(outputFileInfo.filePath());
	if(outputFile.open(QIODevice::WriteOnly))
	{	for(iit = fileNames.begin(); iit != fileNames.end(); iit++)
		{	QString deb = iit->name + " " + iit->filePath + "\n";
			outputFile.write(deb.toLatin1());
		}
		outputFile.close();
	}
}

// read CPP's corresponding XML file, pull out location, add to index
bool extractCPPlocationFromXML(QString &filesXMLfileName, cDoxEnviron &doxEnviron, cDoxIndexEntry &xit)
{	cDoxIndexEntry wkFile;
	QString err;
	QString errorStr;
	int errorLine, errorColumn;

	QFile xmlFile(filesXMLfileName);
	if (!xmlFile.open(QIODevice::ReadOnly))
	{	QString err = "Error unable to open Doxygen XML file " + filesXMLfileName;
		qDebug() << err;
		QMessageBox(QMessageBox::Critical, "Error", err).exec();
		return false;
	}

	QDomDocument doc("DOXfile");
	if (!doc.setContent(&xmlFile, false, &errorStr, &errorLine, &errorColumn))
	{	QString err("Error parsing " + filesXMLfileName + " ");
		err += QString("%1 %2 %2").arg(errorStr).arg(errorLine).arg(errorColumn);
		qDebug() << err;
		QMessageBox(QMessageBox::Critical, "Error", err).exec();
		return false;
	}

	// 'topList' only contains one element, named "doxygen"
	QDomNodeList topList = doc.elementsByTagName("doxygen");
	for(int i = 0; i < topList.size(); i++)
	{	//QDomNode compoundNode = memberdefNode.namedItem("compounddef");

		QDomNodeList compoundList = topList.at(i).childNodes();
		for(int j = 0; j < compoundList.size(); j++)
		{	if(compoundList.at(j).nodeName() != "compounddef")
				continue;
			// compounddef
			// compounddef.location
			QDomNode compoundnameNode = compoundList.at(j).namedItem("location");
			if(!compoundnameNode.isNull())
			{	QDomNamedNodeMap locAttribs = compoundnameNode.attributes();
				if(locAttribs.contains("file"))
				{	QDomNode file = locAttribs.namedItem("file");
					xit.filePath = file.nodeValue();
					QFileInfo fileInfo(xit.filePath);
					if(fileInfo.isRelative())
					{	// does not work if "FULL_PATH_NAMES = NO", we get "sandbox/CodeBase/PG012.cpp"
						// converts to "C:/Progeny/CC_QT/sandbox/CodeBase/PG012.cpp"
						xit.filePath = QFileInfo(doxEnviron.configFileDir, xit.filePath).absoluteFilePath();
					}
					//qDebug() << "(extractCPPlocationFromXML)" << filesXMLfileName << xit.name << xit.filePath;
				}
			}
		}
	}
	return true;
}