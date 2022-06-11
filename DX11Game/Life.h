#pragma once
#include "UI.h"

class Life :
	public UI
{
public:
	HRESULT Init();
	void Uninit();
	void Update();
	void Draw();

	void Set(float fLife);
	void Vib(float fLife);
	void Beat(float fLife);
	void Dance(float fLife);
private:

};

