#ifndef DefinedMVAs_HPP
#define DefinedMVAs_HPP 1

// STL include
#include <vector>
#include <map>

// boosst include
#include <boost/assign/list_of.hpp>

namespace NN {
   									    // --- Cut optimisation
  static const std::map< std::string, std::string > DefinedMVAs = boost::assign::map_list_of("Cuts", "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart") ("CutsD", "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=Decorrelate") ("CutsPCA", "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=PCA") ("CutsGA", "H:!V:FitMethod=GA:CutRangeMin[0]=-10:CutRangeMax[0]=10:VarProp[1]=FMax:EffSel:Steps=30:Cycles=3:PopSize=400:SC_steps=10:SC_rate=5:SC_factor=0.95") ("CutsSA", "!H:!V:FitMethod=SA:EffSel:MaxCalls=150000:KernelTemp=IncAdaptive:InitialTemp=1e+6:MinTemp=1e-6:Eps=1e-10:UseDefaultScale") 
   									    // --- 1-dimensional likelihood ("naive Bayes estimator")
   									    ("Likelihood", "H:!V:TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmoothBkg[1]=10:NSmooth=1:NAvEvtPerBin=50") ("LikelihoodD", "!H:!V:TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmooth=5:NAvEvtPerBin=50:VarTransform=Decorrelate") ("LikelihoodPCA", "!H:!V:!TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmooth=5:NAvEvtPerBin=50:VarTransform=PCA") ("LikelihoodKDE", "!H:!V:!TransformOutput:PDFInterpol=KDE:KDEtype=Gauss:KDEiter=Adaptive:KDEFineFactor=0.3:KDEborder=None:NAvEvtPerBin=50") ("LikelihoodMIX", "!H:!V:!TransformOutput:PDFInterpolSig[0]=KDE:PDFInterpolBkg[0]=KDE:PDFInterpolSig[1]=KDE:PDFInterpolBkg[1]=KDE:PDFInterpolSig[2]=Spline2:PDFInterpolBkg[2]=Spline2:PDFInterpolSig[3]=Spline2:PDFInterpolBkg[3]=Spline2:KDEtype=Gauss:KDEiter=Nonadaptive:KDEborder=None:NAvEvtPerBin=50") 
   									    // --- Mutidimensional likelihood and Nearest-Neighbour methods
									    ("PDERS", "!H:!V:NormTree=T:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600") ("PDERSD", "!H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=Decorrelate") ("PDERSPCA", "!H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=PCA") ("PDEFoam", "!H:!V:SigBgSeparate=F:TailCut=0.001:VolFrac=0.0666:nActiveCells=500:nSampl=2000:nBin=5:Nmin=100:Kernel=None:Compress=T") ("PDEFoamBoost", "!H:!V:Boost_Num=30:Boost_Transform=linear:SigBgSeparate=F:MaxDepth=4:UseYesNoCell=T:DTLogic=MisClassificationError:FillFoamWithOrigWeights=F:TailCut=0:nActiveCells=500:nBin=20:Nmin=400:Kernel=None:Compress=T") ("KNN", "H:nkNN=20:ScaleFrac=0.8:SigmaFact=1.0:Kernel=Gaus:UseKernel=F:UseWeight=T:!Trim")
   									    // --- Linear Discriminant Analysis
									    ("LD", "H:!V:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10") ("Fisher", "H:!V:Fisher:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10") ("FisherG", "H:!V:VarTransform=Gauss") ("BoostedFisher", "H:!V:Boost_Num=20:Boost_Transform=log:Boost_Type=AdaBoost:Boost_AdaBoostBeta=0.2:!Boost_DetailedMonitoring") ("HMatrix", "!H:!V:VarTransform=None")
   									    // --- Neural Networks (all are feed-forward Multilayer Perceptrons)
									    ("MLP", "H:!V:NeuronType=tanh:VarTransform=D:NCycles=500:HiddenLayers=N+1,N:TestRate=25:!UseRegulator:CreateMVAPdfs") ("MLPBFGS", "H:!V:NeuronType=tanh:VarTransform=D:NCycles=500:HiddenLayers=N+1,N:TestRate=25:TrainingMethod=BFGS:!UseRegulator:CreateMVAPdfs") ("MLPBNN", "H:!V:NeuronType=tanh:VarTransform=D:NCycles=500:HiddenLayers=N+1,N:TestRate=25:TrainingMethod=BFGS:UseRegulator:CreateMVAPdfs") ("CFMlpANN", "!H:!V:NCycles=2000:HiddenLayers=N+1,N:CreateMVAPdfs") ("TMlpANN", "!H:!V:NCycles=200:HiddenLayers=N+1,N:CreateMVAPdfs:LearningMethod=BFGS:ValidationFraction=0.3:VarTransform=D,P")
   									    // --- Support Vector Machine 
									    ("SVM", "Gamma=0.25:Tol=0.001:VarTransform=Norm")
   									    // --- Boosted Decision Trees
									    ("BDT", "!H:!V:NTrees=850:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20") ("BDTG", "!H:!V:NTrees=1000:BoostType=Grad:Shrinkage=0.10:UseBaggedGrad:GradBaggingFraction=0.5:nCuts=20:MaxDepth=2") ("BDTB", "!H:!V:NTrees=400:BoostType=Bagging:SeparationType=GiniIndex:nCuts=20") ("BDTD", "!H:!V:NTrees=400:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:VarTransform=Decorrelate:CreateMVAPdfs"); 


  // We can also list additional basic settings here for different modes if we wanted

  static const std::map< std::string, std::string > splitOptions = boost::assign::map_list_of("default", "nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V" ); 

} // end NN namespace
#endif // DefinedMVAs_HPP
