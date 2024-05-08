//Thêm thư viện thích hợp
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <fstream>

//Kích thước cửa số
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

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

//Texture hiện tại xuất hiện
SDL_Texture* gTexture = NULL;

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
	gTexture = loadTexture("src/background.jpg");
	if (gTexture == NULL)
	{
		printf("Failed to load texture image!\n");
		success = false;
	}

	return success;
}

void close()
{
	//Giải phóng bộ nhớ
	SDL_DestroyTexture(gTexture);
	gTexture = NULL;

	//Xóa cửa sổ
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//thoát hệ thống con
	IMG_Quit();
	SDL_Quit();
}

SDL_Texture* loadTexture(std::string path)
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
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

				//Vẽ ảnh lên màn hình
				SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);

				//Cập nhật màn hình
				SDL_RenderPresent(gRenderer);
			}
		}
	}

	//Giải phóng bộ nhớ và đóng chương trình
	close();

	return 0;
}
