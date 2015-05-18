#include "SMFPGroup.h"
#include "SMWord.h"
#include "SMMap.h"
#include "SMSubgroup.h"
#include "SMEquation.h"

void registerProperties()
{
  PropertiesManager* pm = PropertiesManager::instance();

  Chars chars;
  Word word;

  //----------------------------------------------------------------------
  //
  //  GIC
  //
  //----------------------------------------------------------------------

  {
    FreeGroup F;
    SGofFreeGroup H(F, VectorOf<Word>());
    Map map(F,F);
    FPGroup fpGroup;
    AbelianGroup abGroup(fpGroup,false);
    FreeByCyclic freeByCyclic(F,map);
    AmalgProductOfFreeGroups aproduct;
    HNNExtOfFreeGroup hnn(F,"t",H,H);
    
    SubgroupGraph graph(0,SetOf<Word>());
    KBMachine kbmachine;
    GroupDFSA dfsa;
    DiffMachine diff;
    PermutationRepresentation permRep;
    DecomposeInSubgroupOfFPGroup wordDecomposer(fpGroup,VectorOf<Word>());
    AssociationsOf<int,NilpotentGroup*> nilpQuots;
  
    pm -> registerProperty( GroupOrderProperty           ( 0, chars       ) );
    pm -> registerProperty( SolvedWordProblemProperty    ( chars          ) );
    pm -> registerProperty( FastWordProblemProperty      ( chars          ) );
    pm -> registerProperty( CompleteCayleyGraphProperty  ( graph          ) );
    pm -> registerProperty( ConfluentKBMachineProperty   ( kbmachine      ) );
    pm -> registerProperty( IsAutomaticProperty          ( chars          ) );
    pm -> registerProperty( Automatic_GroupDFSAProperty  ( dfsa           ) );
    pm -> registerProperty( Automatic_DiffMachineProperty( diff           ) );
    pm -> registerProperty( OneRelatorProperty           ( word           ) );
    pm -> registerProperty( OneRelatorWithTorsionProperty(                ) );
    pm -> registerProperty( AbelianPresentationProperty  ( abGroup        ) );
    pm -> registerProperty( IsAbelianProperty            (                ) );
    pm -> registerProperty( IsFreeProperty               ( chars          ) );
    pm -> registerProperty( IsFiniteProperty             ( chars          ) );
    pm -> registerProperty( NilpotencyClassProperty      ( 0              ) );
    pm -> registerProperty( NilpotentQuotientsProperty   ( nilpQuots      ) );
    pm -> registerProperty( IsFreeNilpotentProperty      (                ) );
    pm -> registerProperty( ActualNilpotencyClassProperty( 0              ) );
    pm -> registerProperty( IsFreeByCyclicProperty       ( freeByCyclic   ) );
    pm -> registerProperty( MSCProperty                  ( fpGroup, 0     ) );
    pm -> registerProperty( MSCLambdaProperty            ( 0, chars       ) );
    pm -> registerProperty( APofFreeProperty             ( aproduct       ) );
    pm -> registerProperty( HNNofFreeProperty            ( hnn            ) );
    pm -> registerProperty( SchreierTransversalProperty  ( permRep        ) );
    pm -> registerProperty( WordDecomposerProperty       ( wordDecomposer ) );

  }

  //----------------------------------------------------------------------
  //
  //  WIC
  //
  //----------------------------------------------------------------------
  
  {
    pm -> registerProperty( IsTrivialProperty    ( chars      ) );
    pm -> registerProperty( WordOrderProperty    ( 0          ) );
    pm -> registerProperty( MaximalRootProperty  ( word       ) );
    pm -> registerProperty( MaximalPowerProperty ( 0          ) );
    pm -> registerProperty( CollectedFormProperty( PolyWord() ) );
  }

  //----------------------------------------------------------------------
  //
  //  EIC
  //
  //----------------------------------------------------------------------

  {
    ListOf<Endomorphism> endList;
    ListOf<Automorphism> autList;

    pm -> registerProperty( AllBasicSolutionsProperty    (         ) );
    pm -> registerProperty( AllRegStabGeneratorsProperty (         ) );
    pm -> registerProperty( BasicSolutionsProperty       ( endList ) );
    pm -> registerProperty( RegStabGeneratorsProperty    ( autList ) );
  }

  //----------------------------------------------------------------------
  //
  //  SIC
  //
  //----------------------------------------------------------------------

  {    
    FPGroup fpGroup;
    AbelianGroup abGroup(fpGroup,false);
    AbelianSGPresentation abPres;
    AssociationsOf<int,SGOfNilpotentGroup*> sgQuots;
    SGOfNilpotentGroup H( NilpotentGroup(2,0), VectorOf<Word>(0) );

    pm -> registerProperty( CyclicDecompositionOfFactorProperty (abGroup  ) );
    pm -> registerProperty( AbelianSubgroupPresentationProperty (abPres   ) );
    pm -> registerProperty( IsPureSubgroupProperty              (         ) );
    pm -> registerProperty( IsCentralSubgroupProperty           (         ) );
    pm -> registerProperty( IsNormalSubgroupProperty            (         ) );
    pm -> registerProperty( IsAbelianSubgroupProperty           (         ) );
    pm -> registerProperty( IsTrivialSubgroupProperty           (         ) );
    pm -> registerProperty( IndexOfSubgroupProperty             ( 0       ) );
    pm -> registerProperty( SGNilpotentQuotientsProperty        ( sgQuots ) );
    pm -> registerProperty( SubgroupOfNilpotentGroupProperty    ( H       ) );
  }

  //----------------------------------------------------------------------
  //
  //  MIC
  //
  //----------------------------------------------------------------------
  
  {
    pm -> registerProperty( HomIsMonoProperty   (  ) );
    pm -> registerProperty( HomIsEpiProperty    (  ) );
    pm -> registerProperty( ExtendToHomProperty ("") );
  }
}
