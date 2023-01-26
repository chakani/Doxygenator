# Doxygenator
Automatically inserts Doxygen comment blocks before functions. The Doxygen commands are a template or skeleton, you fill in the details. Saves you hours of drudgery.

Requires: Qt 5.12 or later

[1] Run Doxygen on your source files

[2] Run Doxygenator, browse to your Doxygen configuration file, then select CPP files to annotate with comments.

Ex: 

	int foo(char *bar)

becomes:


	/** \brief (You fill in this part)
	*\param bar (char *)
	* \return int
	* \details 
	*/
	int foo(char *bar)
  
VERY IMPORTANT: in Doxygen Config file, must have:

	GENERATE_XML           = YES
	FULL_PATH_NAMES        = YES
	STRIP_FROM_PATH        = NO
	STRIP_FROM_INC_PATH    = NO
	EXCLUDE                = *_doxbackup.cpp
	XML_PROGRAMLISTING     = NO
	INCLUDE_PATH = (specify all directories with header files)
	PREDEFINED             =  (Copy from Project file)

Doxygenator will extract function and class information from Doxygen's XML files, create a comment block, and insert it directly into your CPP file.

BACKUP YOUR FILES beforehand!

