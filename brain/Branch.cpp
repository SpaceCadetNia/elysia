#include "Platform.hpp"
#include "Branch.hpp"
#include "Synapse.hpp"
namespace Elysia {
Branch::Branch(CellComponent * parent) {
    mParentComponent=parent;
}

ProteinDensity&Branch::getProteinDensityStructure() {
    return mParentComponent->getProteinDensityStructure();
}

void Branch::activateComponent(float signal){
    mActivity += signal;
    if(mActivity > mThreshold){
        mParentComponent->activateComponent(mSignalWeight);
    }
}

void Branch::growSynapse(void){
	Synapse *s;
    mChildSynapses.push_back(s = new Synapse(this));
}

}
