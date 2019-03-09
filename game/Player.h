#pragma once
class Player : public Entity
{
public:
	int Experience = 0;
	bool JustWarped = false;
	int TargetWarp = -1;

	bool HasGun = false;
	int SelectedItem = -1;
	std::vector<HeldItem> Inventory;
	double DamageInflicted = 0;
	Player(std::vector<Entity*>* ents);
	
	~Player();

	std::string GetSelectedItemId()
	{
		if (SelectedItem == -1) return "none";
		else if (Inventory.size() == 0) return "none";
		else
			return Inventory[SelectedItem].Id;
	}
	void AddHealth(double hp)
	{
		Health += hp;
		if (Health > MaxHealth) Health = MaxHealth;
	}
	void AddItem(HeldItem item)
	{
		if (SelectedItem == -1) SelectedItem = 0;

		for (int i = 0; i < Inventory.size(); i++)
		{
			if (item.Id == Inventory[i].Id)
			{
				Inventory[i].Count++;
				return;
			}
		}
		Inventory.push_back(item);
	}
	int HasItem(std::string itemid)
	{
		for (int i = 0; i < Inventory.size(); i++)
		{
			if (Inventory[i].Id == itemid)
			{
				return Inventory[i].Count;
			}
		}
		return 0;
	}
	virtual void Update(double dt)
	{
		bool moving = false;
		if (keys[SDLK_w])
		{
			VelocityY = -MaxVelocity;
			moving = true;
		}
		else if (keys[SDLK_s])
		{
			VelocityY = MaxVelocity;
			moving = true;
		}
		else
			VelocityY = 0;


		if (keys[SDLK_a])
		{
			VelocityX = -MaxVelocity;
			moving = true;
		}
		else if (keys[SDLK_d])
		{
			VelocityX = MaxVelocity;
			moving = true;
		}
		else
			VelocityX = 0;

		X += VelocityX * dt;
		Y += VelocityY * dt;

		if (moving)
		{
			AnimationY = 1;
			AnimationX += 0.005 * dt;
			if (AnimationX > 2) AnimationX = AnimationX - (int)AnimationX;
		}
		else
		{
			AnimationX = 0;
			AnimationY = 0;
		}

		// Draw bullet from player to cursor
		if (SDL_GetTicks() >= NextAttack && NextAttack != -1)
		{
			FireBullet();
			if (Inventory[SelectedItem].Id == "uzi")
				NextAttack = SDL_GetTicks() + 100;
		}
	}

	void FireBullet()
	{
		int x, y;
		SDL_GetMouseState(&x, &y);
		int nx = player->X + player->Width * SCALE / 2;
		int ny = player->Y + player->Height * SCALE / 2;
		Bullet* bl = new Bullet(EntityListPointer, atan2(ny + viewport.Y - y, nx + viewport.X - x) + M_PI);
		bl->LoadImage(renderer, "gfx/" + (std::string)"bullet.bmp");
		bl->X = nx;
		bl->Y = ny;
		EntityListPointer->push_back(bl);
	}
};