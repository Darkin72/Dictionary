//Thêm thư viện thích hợp
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <windows.h>
#include <string>
#include <fstream>
#include <map>
#include <iostream>
#include <vector>
#include <iomanip>


//Kích thước cửa số
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

//Khởi tạo một số biến và hằng số cần thiết
const int totalBtn1 = 2;
const int totalBtn2 = 12;
bool start = 1;
TTF_Font* gFont = NULL;
SDL_Color textColor = { 0, 0, 0 };
std::string inputText;
//Cờ lặp chính
bool quit = false;
int mouseY=-1;
int cur = 0;
//Khởi tạo cửa sổ
bool init();
SDL_Window* gWindow = NULL;

//Tải tài nguyên
bool loadMedia();

//Giải phóng bộ nhớ và đóng cửa sổ
void close();

//Đưa cửa số ra màn hình
SDL_Renderer* gRenderer = NULL;

//map các texture

//tọa độ nút
std::map<std::string, std::pair<int, int>> coorBtn{
	{"btn1",{650,266}},
	{"btn2",{650,430}},
	{"back",{27,0}},
	{"speech",{0,0}},
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

	//Loads image at specified path

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

//khai báo Texture
gTexture bg;
gTexture btn1;
gTexture btn2;
gTexture bg2;
gTexture back;
gTexture speech;
gTexture itemBtn;
std::map<std::string, gTexture> myTextures = {
	{"bg",bg},
	{"btn1", btn1},
	{"btn2", btn2},
	{"bg2", bg2},
	{"back", back},
	{"speech", speech},
	{"itemBtn",itemBtn},
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
gBtn TotalButton1[totalBtn1];
gBtn TotalButton2[totalBtn2];


//Texture text
class Text
{
public:
	//Initializes variables
	Text()
	{
		//Initialize
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;

	}

	//Deallocates memory
	~Text()
	{
		//Deallocate
		free();
	}

	//Creates image from font string
	bool load(std::string textureText, SDL_Color textColor)
	{
		//Get rid of preexisting texture
		free();

		//Render text surface
		SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
		if (textSurface == NULL)
		{
			printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		}
		else
		{
			//Create texture from surface pixels
			mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
			if (mTexture == NULL)
			{
				printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
			}
			else
			{
				//Get image dimensions
				mWidth = textSurface->w;
				mHeight = textSurface->h;
			}

			//Get rid of old surface
			SDL_FreeSurface(textSurface);
		}

		//Return success
		return mTexture != NULL;
	}

	//Deallocates texture
	void free()
	{
		//Free texture if it exists
		if (mTexture != NULL)
		{
			SDL_DestroyTexture(mTexture);
			mTexture = NULL;
			mWidth = 0;
			mHeight = 0;
		}
	}


	//Renders texture at given point
	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE)
	{
		//Set rendering space and render to screen
		SDL_Rect renderQuad = { x, y, mWidth, mHeight };

		//Set clip rendering dimensions
		if (clip != NULL)
		{
			renderQuad.w = clip->w;
			renderQuad.h = clip->h;
		}

		//Render to screen
		SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
	}

	//Gets image dimensions
	int getWidth()
	{
		return mWidth;
	}
	int getHeight()
	{
		return mHeight;
	}

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
};
Text FindWord;
Text item;

struct word {
	std::string name;
	std::string pronounce;
	std::vector<std::string> content;
	void reset() {
		name = "";
		pronounce = "";
		content.clear();
	}
};
word newword,choose;
//vector danh sách item xuất hiện
std::vector<word> itemList;
std::vector<word> copy;
void genItem(std::string inputText, std::vector<word>& itemList) {
	//làm mới danh sách
	cur = 0;
	if (inputText.length() >= 1) {
		std::ifstream inp(std::string("src/") + inputText[0] +std::string(".txt"));
		itemList.clear();
		std::string temp;
		while (std::getline(inp, temp)) {
			if(temp[0]=='@')
			{
				if (!newword.name.empty()) itemList.push_back(newword);
				newword.reset();
				bool p = 0;
				for(int i=1;i<temp.length();i++)
				{
					if (temp[i] == '/') p = 1;
					if(!p)
					{
						newword.name += temp[i];
					}
					else newword.pronounce += temp[i];
				}
			}
			else newword.content.push_back(temp.substr(0, temp.length()));
		}
		if (!newword.name.empty()) itemList.push_back(newword);
		inp.close();
		copy.clear();
		for (auto x : itemList) {
			if (x.name.length() >= inputText.length()) {
				if (inputText == x.name.substr(0, inputText.length())) {
					copy.push_back(x); //giữ lại những từ có phần tử đầu giống inputText
				}
			}
		}
		itemList = copy;
	}
	else 
		itemList.clear();
	while (itemList.size() < 10) {
		itemList.push_back({"",""});
	}
}
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
				//khởi tạo font
				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
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
	myTextures["bg"].load("src/background.jpg");
	myTextures["bg2"].load("src/background2.jpg");
	myTextures["btn1"].load("src/start.jpg");
	myTextures["btn2"].load("src/quit.jpg");
	myTextures["back"].load("src/back.png");
	myTextures["speech"].load("src/speech.png");
	myTextures["itemBtn"].load("src/item.png");
	gFont = TTF_OpenFont("src/lazy.ttf", 44);

	TotalButton1[0].getInf("btn1");
	TotalButton1[1].getInf("btn2");

	TotalButton2[0].getInf("back");
	TotalButton2[1].getInf("speech");
	for (int i = 2; i <= 11; i++) {
		TotalButton2[i].getInf("itemBtn",0,204+(i-2)*50);
	}

	if (gFont == NULL)
	{
		printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}

	return success;
}

void close()
{

	//Xóa cửa sổ
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;
	TTF_CloseFont(gFont);
	gFont = NULL;

	//thoát hệ thống con
	IMG_Quit();
	TTF_Quit();
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
				if (com == "btn2") {
					quit = true;
				} else 
					if (com == "btn1") {
						start = 0;
						inputText = "";
						genItem(inputText, itemList);
					}
					else {
						if (com == "back") {
							start = 1;
							choose.reset();
							mouseY = -1;
							cur = 0;
							itemList.clear();
							copy.clear();
						}
						else if (com == "speech") {
								std::string command = "espeak \"" + inputText + "\"";
								const char* charCommand = command.c_str();
								system(charCommand);
						}
						else if (com == "itemBtn") {
								choose = itemList[(mPos.y - 204) / 50 + cur];
						}
						
					}
				break;
			default:
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

			//Xử lý sự kiện
			SDL_Event e;
			//The current input text.

			//Enable text input
			SDL_StartTextInput();

			//Trong khi chương trình chạy
			while (!quit)
			{
				//Xử lý sự kiện
				while (SDL_PollEvent(&e) != 0)
				{
					//Người dùng bấm nút
					if (start)
						for (int i = 0; i < totalBtn1; i++) {
							TotalButton1[i].handleEvent(&e);
						}
					else {
						for (int i = 0; i < totalBtn2; i++) {
							TotalButton2[i].handleEvent(&e);
							SDL_Delay(1);
						}
						int x, y;
						SDL_GetMouseState(&x, &y);
						if (y > 204 && x < 300) { //nằm trong area itemList
							mouseY = (y - 204) / 50;
							if (e.type == SDL_MOUSEWHEEL) {
								if (e.wheel.y > 0) { // lăn chuột lên
									if (cur > 0) --cur;
								}
								else if (e.wheel.y < 0) { //lăn chuột xuống
									if (cur < (int)itemList.size() - 10) ++cur;
								}
							}
						} else mouseY = -1;
							
						if (e.type == SDL_KEYDOWN)
						{
							//Handle backspace
							if (e.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0)
							{
								//lop off character
								inputText.pop_back();
								copy.clear();
								genItem(inputText, itemList);
							}
							//Handle copy
							else if (e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
							{
								SDL_SetClipboardText(inputText.c_str());
							}
							//Handle paste
							else if (e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
							{
								//Copy text from temporary buffer
								char* tempText = SDL_GetClipboardText();
								inputText = tempText;
								SDL_free(tempText);
							}
					}
					//Special text input event
						else if (e.type == SDL_TEXTINPUT)
						{
							//Not copy or pasting
							if (!(SDL_GetModState() & KMOD_CTRL && (e.text.text[0] == 'c' || e.text.text[0] == 'C' || e.text.text[0] == 'v' || e.text.text[0] == 'V')))
							{
								//Append character
								inputText += e.text.text;
								genItem(inputText, itemList);
							}
					}
					}
					//Người dùng thoát
					if (e.type == SDL_QUIT)
					{
						quit = true;
					} 
				}

				//Xóa màn hình
				SDL_RenderClear(gRenderer);
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				// Rerender text if needed
				//Text is not empty
				if (inputText != "")
				{
					//Render new text
					FindWord.load(inputText.c_str(), textColor);
				}
				//Text is empty
				else
				{
					//Render space texture
					FindWord.load("_", textColor);
				}
				//Render text textures
				if(start)
				{
					myTextures["bg"].render(0, 0);
					myTextures["btn1"].render(650, 266);
					myTextures["btn2"].render(650, 430);
				}
				else {
					myTextures["bg2"].render(0, 0);
					myTextures["speech"].render(0, 0);
					FindWord.render(404, 0);
					if (itemList.size() != 0) {
						if (mouseY >= 0 && mouseY < copy.size()) myTextures["itemBtn"].render(0, 50 * mouseY + 204);
						for (int i=0;i<min(10,itemList.size()-cur);i++)
							{
								if (itemList[i + cur].name != "")
								{
									item.load(itemList[i + cur].name, textColor);
									item.render(0, 50 * i + 192);
								}
								else break;
						}
					}
					std::cout << copy.size() << '\n';
					if (choose.name!="") {
						item.load(choose.name,textColor);
						item.render(350, 192);
						item.load(choose.pronounce, textColor);
						item.render(350, 242);
						for (int i = 0; i < choose.content.size(); i++) {
							item.load(choose.content[i], textColor);
							item.render(350, 292 + i * 50);
						}
					}
				}
				//Cập nhật màn hình
				SDL_RenderPresent(gRenderer);
			}
		}
	}

	//Giải phóng bộ nhớ và đóng chương trình
	close();

	return 0;
}
