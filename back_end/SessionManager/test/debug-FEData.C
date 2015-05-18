#include "FEData.h"



struct Foo : public FEData
{
  void foo( const Expression& A )
  {
	 A.printVar( cout );
	 cout << endl;
	 
	 A.printConst( cout );
	 cout << endl << endl;
  }

  void test1( ) { foo( IsIso(0) && Domain(0) == Object(1) ); }
  void test2( ) { foo( IsIso(0) && ! Domain(0) == Object(1) ); }
  void test3( ) { foo( IsIso(0) && ! ( Domain(0) == Object(1) ) ); }

};





main( )
{
  Foo F;

  F.test1();
  F.test2();
  F.test3();

  FEDataUpdate msg( FEData::IsIso( 4 ), FEData::Object( 1 ) );
  msg.send();
}
