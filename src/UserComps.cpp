#include "../header/int/UserComps.h"

void LadderComponent::OnUpdate(){
	Vector3 playerPos = playerObj->transform.GlobalPosition();
	if(col->Contains(playerPos)){
		playerControl->AttachToLadder(this);
	}
}