#include "Margin.h"
#include "OneRelatorGroup.h"
#include "SubgroupOfOneRelatorGroup.h"
#include "HNNExtOfORGroup.h"

Margin globalMargin;

int rightMargin = 5 * 2;

void Margin::set( Margin& t )
{ 
  if( ptr ) {
    warn(" margin set duplicated");
    return;
  }

  *(Chars *)this = (Chars)t;
  (Chars &)t = (Chars &)t + "  ";
  ptr = &t;
}

void debugPrint( ostream& ostr, const OneRelatorGroup& G ) 
{
  ostr << "< ";
  bool comma = false;
  for( int i = 1; i <= G.numberOfGenerators(); ++i ) {
    if( comma ) 
      ostr << ", ";
    else
      comma = true;
    ostr << i;
  }
  ostr <<"; " << G.relator() << " >";
}

void debugPrint( ostream& ostr, const OneRelatorGroupRep& G ) 
{
  ostr << "< ";
  bool comma = false;
  for( int i = 1; i <= G.theNumberOfGenerators; ++i ) {
    if( comma ) 
      ostr << ", ";
    else
      comma = true;
    ostr << i;
  }
  ostr <<"; " << G.theRelator << " >";
}

void debugPrint( ostream& ostr, const HNNExtOfORGroupGeneric& H ) 
{
  ostr << "< ";
  debugPrint( ostr, H.basisGroup() );
  ostr << ", " << H.basisGroup().numberOfGenerators()+1 << " ; "
       << " A = " << H.subgroup(HNNExtOfORGroupGeneric::A).generators() << " ; " 
       << " B = " << H.subgroup(HNNExtOfORGroupGeneric::B).generators() << " >";
}

void debugPrint( ostream& ostr, const ORGSubgroup& S )
{
  ostr << "gp(" << S.generators() << " ; ";
  debugPrint( ostr, S.parentGroup() );
  ostr << ')';  
}

void debugPrint( ostream& ostr, const SubgroupOfOneRelatorGroup& S )
{
  ostr << "gp(" << S.generators() << " ; ";
  debugPrint( ostr, S.parentGroup() );
  ostr << ')';  
}

