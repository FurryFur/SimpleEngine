#pragma once

#include "System.h"

class SimpleWorldSpaceMoveSystem : public System {
	// Inherited via System
	virtual void update(Entity &) override;
	virtual void beginFrame() override;
	virtual void endFrame() override;
};