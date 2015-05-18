
#include "PackagesSMApps.h"



PackageBlackBox::PackageBlackBox(PObject* po, const Chars& command, const ListOf<Chars>& pl):
  theCommand( command ),
  package(command),
  theObject( po ),
  paramList( pl )
{

}

bool PackageBlackBox::start()
{
  if (!package.start())
    return false;

  ostream& out = package.toCommand();


 //  for (int i=0;i<initString.length();i++)
//     if (initString[i] == '@')
//       out << endl;
//     else
//       out << initString[i];
  theObject->writeInitString(out);
  
  ListIterator<ListOf<Chars> > I(paramList);
  for (I.reset();!I.done();I.next()){
    out << I.value() << endl;
  }
  //   ofstream out2("/tmp/out");
  //   out2 << initString << flush;
//   out2.close();
  return true;
}


bool PackageBlackBox::finished()
{ 
  return  package.fromCommand().eof();
}

bool PackageBlackBox::getLine( Chars& line )
{
  char theLine[1000];
  istream& in = package.fromCommand();

  in.getline(theLine,1000);

  if (strlen(theLine) == 0)
    return false;
 
  line = Chars( theLine);
    
  return true;
}



