BEGIN {
 print "_KBExtDir := Concatenation(LOADED_PACKAGES.kbmag,\"bin/`hostname`/\");"
 print
}

/^[A-Za-z0-9_]+ *:=/ {
    if ( name != FILENAME )
    {
        if ( line != "" )
            print line " );\n";
        line = "AUTO( ReadPkg( \"kbmag\", \"gap\", \"";
        name = substr( FILENAME, 1, index(FILENAME,".")-1 );
        line = line name "\" )";
        print line ",";
        funcs[$1] = name;
        line = "  " $1;
        name = FILENAME;
    }
    else if ( 77 < length( line ", " $1 ) )
    {
        print line ",";
        funcs[$1] = name;
        line = "  " $1;
        frst = 0;
    }
    else
    {
        funcs[$1] = name;
        line = line ", " $1;
    }
}

END {
    print line " );\n";
}




