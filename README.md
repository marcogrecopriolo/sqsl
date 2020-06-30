Structured Query Scripting Language
===================================

Over the years a the world has seen the rise of a plethora of domain specific and generalized scripting languages. Although most of them offer some sort of database engine hook, a seamless SQL integration in the host language, be it in the form of embedded SQL or sympathetic grammar, is never in the cards. With the lack of a DBA domain specific scripting language, DBAs have to put up with either using multiple tools or clumsy language and operating system interaction (or both) in order to be able to pass statements to a database engine and get rows back.

The Structured Query Scripting Language aims to bridge this gap, bringing the ability to script in a SQL manner from outside the database engine, offering seamless connection, control and formatting constructs and interaction with the operating system. With syntax features comparable to the SQL language itself, the language is designed to be obvious to anyone with SQL, PL/SQL, T-SQL, Informix SPL or x4GL expertise.

A bit of history
----------------

Back in the day, when in charge of designing and developing the ERP system at my first employer, people would often come to me and ask to implement a simple report which would consist at most of a couple of SELECT statements, minimal glue code and some formatting.
Simple task you would think, except that the publishing cycle for the application, considering the powerful technological means at our disposal,would be at the very least 24 hours, if not 48.
Wouldn't it be nice, I said to myself (yes, being a geek can be a very lonely affair), if I could just store the sql and formatting somewhere in the database, and have a scripting language runner do the rest of the work for me, or rather, my users?
A few late nights later a prototype implementation had been deployed, with minimal control syntax, an expansion facility and formatting capabilities. SQSL was born.

A domain specific problem example
---------------------------------

Suppose data needs to be migrated between two different database engines, be it on a regular basis, or just as a one off. Suppose also that there is a requirement to calculate aggregates.
Several approaches spring to mind: for example unload to file from the source database engine, load from file to the target engine and finally update the data and obtain aggregates through simple SQL. Although feasible, the drawbacks of such an approach are many, and obvious.
A more effective alternative would be to write small application in java, perl or python, which selects from one source, calculates aggregates and finally inserts in the target, although the complexity of interacting with two SQL sources from your chosen host language might not make this a simple to implement or efficient proposition.

Or you could do it the SQSL way:

	CONNECT TO "db1" SOURCE db2;
	CONNECT TO "db2" SOURCE ifmx;
	LET min=1000000;
	LET max=0;
	LET avg=0::float;
	LET rows=0;
	SELECT col1, col2, col3, col4
	  FROM tab1
	  CONNECTION "db1"
	  AGGREGATE rows=rows+1, avg=avg+(($1-avg)/rows)::float, ($1>max) max=$1, ($1<min) min=$1
	  INSERT INTO tab1
	    VALUES (?, ?, ?, ?)
	    CONNECTION "db2";
	DISPLAY min, max, avg, rows
	  FORMAT FULL "%08d %08d %10f %08d"
	    HEADERS "min", "max", "average", "nrows";
    
and if you wanted to speed up things, you maybe could

	CLONE 10 INTO child, pid(child);
	  CONNECT TO "db1" SOURCE db2;
	  CONNECT TO "db2" SOURCE ifmx;
	  SELECT * FROM tab1
 	    WHERE col1>? AND col1<? USING (child-1)*100000, child*100000
 	    CONNECTION "db1"
	    INSERT INTO tab1 VALUES (?, ?, ?, ?)
	      CONNECTION "db2";
	DONE;
	WAIT FOR pid.* INTO r, d;
	DONE;

In addition to the above, and aside from standard control and simple statements, the language sports a number of other useful features such as a simple but effective expansion facility and the ability to load dynamically data sources or user defined functions. All of them are detailed in the language reference and demonstrated in the following sample scripts.

The language is implemented in the form a of a library (two API flavours available: c or x4gl) which can be used to plug the interpreter in your application of choice, and a couple of sample clients, of which one has a readline library based interface and the other curses (in two implementations: Informix 4gl and Aubit4gl). There are skeletal NPAPI and Node.js wrappers with demo shells implemented as ACE based web pages and NWJS apps. These are not yet ready for public consumption.

Connectivity is currently provided, in the form of shared objects, for the Informix, DB2, SolidDB and Couchbase engines

Further reading
---------------

- The [language reference](http://www.sqsl.org/sqslref.htm)
- A collection of [sample scripts](http://www.sqsl.org/sqsldemo.htm)
- The [demo clients reference](http://www.sqsl.org/sqslcli.htm)
