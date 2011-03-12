#include "AnnSpace.hpp"


namespace Elysia{


	AnnSpace::AnnSpace(){
		child[0] = NULL;
		child[1] = NULL;
		parent = NULL;
	}

	AnnSpace::~AnnSpace(){
		if(child[0] != NULL){
			delete child[0];
		}
		if(child[1] != NULL){
			delete child[1];
		}
	}

	bool AnnSpace::isLeaf(){
		if (child[0] == NULL){
			assert(child[1] == NULL);
			return true;
		}
		else{
			return false;
		}
	}

	int AnnSpace::chooseChild(float x, float y){
		if(whichAxis == XAXIS){
			if(x > partitionPoint){return 1;}
			else{return 0;}
		}
		else{
			if(y  > partitionPoint){return 1;}
			else{return 0;}
		}
	}

	class ComparePlaceablebyY {
		public:
		bool operator() (const Placeable*a, const Placeable*b) const {
		return a->getLocation().y<b->getLocation().y;
		}
	};
	class ComparePlaceablebyX {
		public:
		bool operator() (const Placeable*a, const Placeable*b) const {
		return a->getLocation().x<b->getLocation().x;
		}
	};

	void AnnSpace::partitionSpace(TreeNNSpatialSearch* treenn){
		std::vector<float> XArray;
		std::vector<float> YArray;
		for(std::vector<Placeable*>::iterator i=placeableList.begin();i!=placeableList.end();++i){
			Placeable* current=*i;
			Vector3f location  = current->getLocation();
			XArray.push_back(location.x);
			YArray.push_back(location.y);
		}
		child[0] = new AnnSpace();
		child[1] = new AnnSpace();
		child[0]->setParent(this);
		child[1]->setParent(this);
		//This algorithm is a super primitive way of partitioning the space. Something like k-means might be better
		//As the number of points becomes large, I expect the exactness of this for ANN improves and it's not that
		//bad to begin with. If someone wants to put k-means in here (or some other thing for calling partitions), cool.
		sort(XArray.begin(), XArray.end());
		sort(YArray.begin(), YArray.end());
		float Xdist = XArray[int(XArray.size()/2) + 1] - XArray[int(XArray.size()/2)];
		float Ydist = XArray[int(YArray.size()/2) + 1] - YArray[int(YArray.size()/2)];
		
		if(Xdist > Ydist){
			whichAxis = XAXIS;
			std::sort(placeableList.begin(),placeableList.end(),ComparePlaceablebyX()); 
			Placeable* midpoint = placeableList[int(placeableList.size()/2)];
			float partition = (XArray[int(XArray.size()/2) + 1] + XArray[int(XArray.size()/2)])/2;
			partitionPoint = partition;
			for(std::vector<Placeable*>::iterator i=placeableList.begin();i!=placeableList.begin()+placeableList.size()/2;++i){

				child[0]->addPoint(*i, treenn);
			}
			for(std::vector<Placeable*>::iterator i=placeableList.begin()+placeableList.size()/2;i!=placeableList.end();++i){
				child[1]->addPoint(*i, treenn);
			}
		}
		else{
			whichAxis = YAXIS;
			std::sort(placeableList.begin(),placeableList.end(),ComparePlaceablebyY()); 
			Placeable* midpoint = placeableList[int(placeableList.size()/2)];
			
			float partition = (YArray[int(YArray.size()/2) + 1] + YArray[int(YArray.size()/2)])/2;
			partitionPoint = partition;
			for(std::vector<Placeable*>::iterator i=placeableList.begin();i!=placeableList.begin()+placeableList.size()/2;++i){
				child[0]->addPoint(*i, treenn);
			}
			for(std::vector<Placeable*>::iterator i=placeableList.begin()+placeableList.size()/2;i!=placeableList.end();++i){
				child[1]->addPoint(*i, treenn);
			}
		}
		placeableList.clear();
	}

	void AnnSpace::deletePoint(Placeable* placeable, TreeNNSpatialSearch* treenn){
		if(this->isLeaf()){
			std::vector<Placeable*>::iterator result = std::find(placeableList.begin(), placeableList.end(), placeable);
			placeableList.erase(result);
			if(placeableList.size() < treenn->pointLowerThreshold){
				if(parent != NULL){
					parent -> mergeSpace(treenn);
				}
			}
		}
		else{
			Vector3f location = placeable->getLocation();
			child[chooseChild(location.x, location.y)] -> addPoint(placeable, treenn);
		}
	}


	void AnnSpace::addPoint(Placeable* placeable, TreeNNSpatialSearch* treenn){
		if(this->isLeaf()){
			placeableList.push_back(placeable);
			if(placeableList.size() + 1 > treenn->pointUpperThreshold){
				this->partitionSpace(treenn);
			}
		}
		else{		
			Vector3f location = placeable->getLocation();
			child[chooseChild(location.x, location.y)] -> addPoint(placeable, treenn);
		}
	}


	Placeable* AnnSpace::findNN(float x, float y, Placeable* exclude){	
		if(this->isLeaf()){
			Vector3f queryPoint;
			queryPoint.x = x;
			queryPoint.y = y;
			queryPoint.z = 0;
			float maxDistance;
			Placeable * maxDistanceItem=NULL;
			for(std::vector<Placeable*>::iterator i=placeableList.begin();i!=placeableList.end();++i) {
				Placeable * current=*i;//find out what's "inside" i
				float currentDistance=(current->getLocation()-queryPoint).length();
				if ((maxDistanceItem==NULL || currentDistance<maxDistance) && current != exclude){
					maxDistance=currentDistance;
					maxDistanceItem=current;
				}
			}
			return maxDistanceItem;
		}
		else{
			return child[chooseChild(x, y)] -> findNN(x, y, exclude);
		}
	}

	void AnnSpace::mergeSpace(TreeNNSpatialSearch* treenn){
		partitionPoint = 0.0f;
		std::vector<Placeable*> newplaceableList;
		newplaceableList = child[0]->getChildList();
		for(std::vector<Placeable*>::iterator i=placeableList.begin();i!=placeableList.end();++i) {
			Placeable* current=*i;
			this->addPoint(current, treenn);
		}
		newplaceableList = child[1]->getChildList();
		for(std::vector<Placeable*>::iterator i=placeableList.begin();i!=placeableList.end();++i) {
			Placeable* current=*i;
			this->addPoint(current, treenn);
		}
		delete child[0];
		delete child[1];
	}

	std::vector<Placeable*> AnnSpace::getChildList(){
		return placeableList;
	}
		
}