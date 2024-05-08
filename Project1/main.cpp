//Thêm thư viện thích hợp
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <map>

//Kích thước cửa số
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

//Khởi tạo một số hằng số cần thiết
const int totalBtn = 1;
//Khởi tạo cửa sổ
bool init();
SDL_Window* gWindow = NULL;

//Tải tài nguyên
bool loadMedia();

//Giải phóng bộ nhớ và đóng cửa sổ
void close();

//Tải ảnh lên như 1 texture
SDL_Texture* loadTexture(std::string path);


//Đưa cửa số ra màn hình
SDL_Renderer* gRenderer = NULL;

//map các texture

//tọa độ nút
std::map<std::string, std::pair<int, int>> coorBtn{
	{"btn1",{628,266}},
	{"btn2",{628,430}},
	
};
//class Texture
class gTexture
{
public:
	//Initializes variables
	gTexture() {
		success = true;
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
		xA = 0;
		yA = 0;
	}
	void load(std::string path) {
		success = true;
		free();
		SDL_Texture* newTexture = NULL;
		newTexture = IMG_LoadTexture(gRenderer, path.c_str());
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
			success = false;
		}
		else
		{
			SDL_QueryTexture(newTexture, NULL, NULL, &mWidth, &mHeight);
		}
		mTexture = newTexture;
	}
	//Deallocates memory
	~gTexture() {
		//Deallocate
		free();
	}

	//Deallocates texture
	void free() {
		// Free texture if it exists
		if (mTexture != NULL)
		{
			SDL_DestroyTexture(mTexture);
			mTexture = NULL;
			mWidth = 0;
			mHeight = 0;
		}
	}

	//Renders texture at given point
	void render(int x, int y) {
		SDL_Rect box = { x, y, mWidth, mHeight };
		setXA(x);
		setYA(y);
		SDL_RenderCopy(gRenderer, mTexture, NULL, &box);
	}

	//Gets image dimensions
	int getWidth() {
		return mWidth;
	}
	int getHeight() {
		return mHeight;
	}
	void setXA(int x) {
		xA = x;
	}

	void setYA(int y) {
		xA = y;
	}
	int getXA() {
		return xA;
	}
	int getYA() {
		return yA;
	}
	bool check() {
		return success;
	}
	void setBlendMode(SDL_BlendMode blending)
	{
		//Set blending function
		SDL_SetTextureBlendMode(mTexture, blending);
	}
	void setColor(int x, int y, int z) {
		SDL_SetTextureColorMod(mTexture, x, y, z);
	}
private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	bool success;
	int mWidth;
	int mHeight;
	int xA;
	int yA;
};

//background
gTexture bg;
gTexture btn1;
gTexture btn2;
std::map<std::string, gTexture> myTextures = {
	{"bg",bg},
	{"btn1", btn1},
	{"btn2", btn2},
};
class gBtn {
public:
	gBtn() {
		mPos.x = 0;
		mPos.y = 0;
		mPos.w = 0;
		mPos.h = 0;
		com = "";
	}
	void getInf(std::string temp, int x = -1, int y = -1) {
		if (x == -1)
		{
			mPos.x = coorBtn[temp].first;
			mPos.y = coorBtn[temp].second;
		}
		else {
			mPos.x = x;
			mPos.y = y;
		}
		mPos.w = myTextures[temp].getWidth();
		mPos.h = myTextures[temp].getHeight();
		com = temp;
	}
	void handleEvent(SDL_Event* e);
private:
	SDL_Rect mPos;
	std::string com;
};


bool init()
{
	//Khởi tạo cờ
	bool success = true;

	//Khởi tạo SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Tạo cửa sổ
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Tạo render cho cửa sổ
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Khởi tạo màu
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Khởi tạo ảnh
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}
		}
	}
	
	return success;
}

bool loadMedia()
{
	//Khởi tạo cờ
	bool success = true;

	//Tải tài nguyên
	bg.load("src/image1.jpg");
	
	return success;
}

void close()
{

	//Xóa cửa sổ
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//thoát hệ thống con
	IMG_Quit();
	SDL_Quit();
}
void gBtn::handleEvent(SDL_Event* e)
{
	if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
	{
		//Get mouse position
		int x, y;
		SDL_GetMouseState(&x, &y);
		//Check if mouse is in button
		bool inside = true;

		//Mouse is left of the button
		if (x < mPos.x)
		{
			inside = false;
		}
		//Mouse is right of the button
		else if (x > mPos.x + mPos.w)
		{
			inside = false;
		}
		//Mouse above the button
		else if (y < mPos.y)
		{
			inside = false;
		}
		//Mouse below the button
		else if (y > mPos.y + mPos.h)
		{
			inside = false;
		}
		//Mouse is outside button
		if (!inside)
		{
			//place holder
		}
		//Mouse is inside button
		else
		{
			//Set mouse over sprite
			switch (e->type)
			{
				/*case SDL_MOUSEMOTION:

					break;*/

				case SDL_MOUSEBUTTONDOWN:
					break;
			}
		}
	}
}
int main(int argc, char* args[])
{
	//Bắt đầu SDL và tạo cửa sổ
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Tải tài nguyên
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//Cờ lặp chính
			bool quit = false;

			//Xử lý sự kiện
			SDL_Event e;

			//Trong khi chương trình chạy
			while (!quit)
			{
				//Xử lý sự kiện
				while (SDL_PollEvent(&e) != 0)
				{
					//Người dùng thoát
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
					
				}

				//Xóa màn hình
				SDL_RenderClear(gRenderer);
				bg.render(0, 0);

				//Cập nhật màn hình
				SDL_RenderPresent(gRenderer);
			}
		}
	}

	//Giải phóng bộ nhớ và đóng chương trình
	close();

	return 0;
}
