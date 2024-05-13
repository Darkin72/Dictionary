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
#include <fcntl.h>
#include <io.h>  
#include <locale>
#include <codecvt>


//Kích thước cửa số
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

//Khởi tạo một số biến và hằng số cần thiết
const int totalBtn1 = 2;
const int totalBtn2 = 12;
bool start = 1;
TTF_Font* gFont = NULL;
TTF_Font* contentFont = NULL;
SDL_Color textColor = { 0, 0, 0 };
std::string inputText;
Uint8 display_number = 0;
//khởi tạo đọc unicode
std::locale loc(std::locale(), new std::codecvt_utf8<wchar_t>);
//Cờ lặp chính
bool quit = false;
int mouseY = -1;
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
};
//class Texture
class gTexture
{
public:
	//hàm khởi tạo 1 texture cho class gTexture
	gTexture() {
		success = true;
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
		xA = 0;
		yA = 0;
	}
	//tải lên theo đường dẫn sử dụng IMG_LoadTexture
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
	//deallocator tự động xóa những thứ được khởi tạo, trái ngược với gTexture()
	~gTexture() {
		free();
	}
	//hàm giải phóng texture
	void free() {
		if (mTexture != NULL)
		{
			SDL_DestroyTexture(mTexture);
			mTexture = NULL;
			mWidth = 0;
			mHeight = 0;
		}
	}

	//render texture ở tọa độ x y
	void render(int x, int y) {
		SDL_Rect box = { x, y, mWidth, mHeight };
		setXA(x);
		setYA(y);
		SDL_RenderCopy(gRenderer, mTexture, NULL, &box);
	}

	//lấy kích cỡ của texture đc load vào
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
		//hàm khởi tạo blend mode (liên quan tới màu sắc)
		SDL_SetTextureBlendMode(mTexture, blending);
	}
	void setColor(int x, int y, int z) {
		SDL_SetTextureColorMod(mTexture, x, y, z);
	}
private:
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
gTexture itemBtn;
std::map<std::string, gTexture> myTextures = {
	{"bg",bg},
	{"btn1", btn1},
	{"btn2", btn2},
	{"bg2", bg2},
	{"back", back},
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


//class cho các text
class Text
{
public:
	//hàm khởi tạo default
	Text()
	{
		//Initialize
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;

	}

	//tương tự, deallocator của text()
	~Text()
	{
		free();
	}

	//tạo hình ảnh từ 1 string
	bool load(std::string textureText, SDL_Color textColor)
	{
		//giải phóng những thằng đang chứa sẵn nếu có trong texture
		free();

		//render surface
		SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
		if (textSurface == NULL)
		{
			printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		}
		else
		{
			mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
			if (mTexture == NULL)
			{
				printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
			}
			else
			{
				//lấy kích cỡ
				mWidth = textSurface->w;
				mHeight = textSurface->h;
			}

			//giải phóng surface
			SDL_FreeSurface(textSurface);
		}

		//return 1 bool kiểm tra có load được không
		return mTexture != NULL;
	}
	bool specialLoad(std::wstring textureText, SDL_Color textColor)
	{
		free();
		SDL_Surface* textSurface = TTF_RenderUNICODE_Solid(contentFont, (const Uint16*)textureText.c_str(), textColor);
		if (textSurface == NULL)
		{
			printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		}
		else
		{
			mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
			if (mTexture == NULL)
			{
				printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
			}
			else
			{
				mWidth = textSurface->w;
				mHeight = textSurface->h;
			}

			
			SDL_FreeSurface(textSurface);
		}
		return mTexture != NULL;
	}

	void free()
	{
		if (mTexture != NULL)
		{
			SDL_DestroyTexture(mTexture);
			mTexture = NULL;
			mWidth = 0;
			mHeight = 0;
		}
	}


	// hàm render tại tọa độ x y
	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE)
	{
		//khởi tạo ô chứa thứ sẽ được render ra nogaif màn hình
		SDL_Rect renderQuad = { x, y, mWidth, mHeight };

		//
		if (clip != NULL)
		{
			renderQuad.w = clip->w;
			renderQuad.h = clip->h;
		}
		//hàm render ra màn hình
		SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
	}

	// lấy kích cỡ
	int getWidth()
	{
		return mWidth;
	}
	int getHeight()
	{
		return mHeight;
	}

private:
	SDL_Texture* mTexture;
	int mWidth;
	int mHeight;
};
Text FindWord;
Text item;
Text content;

struct word {
	std::string name;
	int contentPos=0;
	void reset() {
		name = "";
		contentPos = 0;
	}
};
word newword, choose;
std::vector<std::wstring> ChosenWord;
//vector danh sách item xuất hiện
std::vector<word> itemList;
std::vector<word> copy;
void genItem(std::string inputText) {
	//làm mới danh sách
	display_number = 0;
	cur = 0;
	if (inputText.length() == 1) {
		std::ifstream inp(std::string("src/") + inputText[0] + ("/name.txt"));
		itemList.clear();
		std::string temp;
		while (std::getline(inp, temp)) {
			newword.name = temp;
			newword.contentPos =(int) itemList.size() + 1;
			itemList.push_back(newword);
		}
		inp.close();
		copy = itemList;
	}
	else if (inputText.length() > 1) {
		copy.clear();
		for (auto x : itemList) {
			if (x.name.length() >= inputText.length()) {
				if (inputText == x.name.substr(0, inputText.length())) {
					copy.push_back(x); //giữ lại những từ có phần tử đầu giống inputText
				}
			}
		}
	}
	else
		itemList.clear();
	display_number =(Uint8) copy.size();
	while (copy.size() < 10) {
		copy.push_back({ "" });
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
	myTextures["itemBtn"].load("src/item.png");
	gFont = TTF_OpenFont("src/lazy.ttf", 44);
	contentFont = TTF_OpenFont("src/lazy.ttf", 20);

	TotalButton1[0].getInf("btn1");
	TotalButton1[1].getInf("btn2");

	TotalButton2[0].getInf("back");
	for (int i = 1; i <= 10; i++) {
		TotalButton2[i].getInf("itemBtn", 0, 204 + (i - 1) * 50);
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
		//lấy vị trí con chuột
		int x, y;
		SDL_GetMouseState(&x, &y);
		//check nếu chuột ở bên trong
		bool inside = true;

		//chuột nằm bên trái
		if (x < mPos.x)
		{
			inside = false;
		}
		//chuột nằm bên phải
		else if (x > mPos.x + mPos.w)
		{
			inside = false;
		}
		//chuột nằm trên
		else if (y < mPos.y)
		{
			inside = false;
		}
		//chuột nằm dưới
		else if (y > mPos.y + mPos.h)
		{
			inside = false;
		}
		//nếu chuột nằm ở ngoài
		if (!inside)
		{
			//place holder
		}
//nếu chuột nằm trong
		else
		{
			switch (e->type)
			{
			case SDL_MOUSEBUTTONDOWN:
				if (com == "btn2") {
					quit = true;
				}
				else
					if (com == "btn1") {
						start = 0;
						inputText = "";
						genItem(inputText);
					}
					else {
						if (com == "back") {
							start = 1;
							choose.reset();
							mouseY = -1;
							cur = 0;
							itemList.clear();
							copy.clear();
							ChosenWord.clear();
						}
						else if (com == "itemBtn") {
							ChosenWord.clear();
							choose = copy[(mPos.y - 204) / 50 + cur];
							std::wifstream fin(std::wstring(L"src/") + (wchar_t)inputText[0] + (L"/content.txt"));
							fin.imbue(loc);
							std::wstring temp;
							int count = 1;
							while (std::getline(fin, temp) && choose.contentPos>=0) {
								if (choose.contentPos == 0)
								{
									ChosenWord.push_back(temp);
								}
								if (temp[0] == L'#')
								{
									--choose.contentPos;
								}
							}
							fin.close();
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
									if (cur < (int)copy.size() - 10) ++cur;
								}
							}
						}
						else mouseY = -1;

						if (e.type == SDL_KEYDOWN)
						{
							//trường hợp xóa ký tự
							if (e.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0)
							{
								inputText.pop_back();
								copy.clear();
								genItem(inputText);
							}
						}
						//Special text input event
						else if (e.type == SDL_TEXTINPUT)
						{
						//Not copy or pasting
							//Append character
							inputText += e.text.text;
							genItem(inputText);
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
				if (inputText != "")
				{
					FindWord.load(inputText.c_str(), textColor);
				}
				else
				{
					FindWord.load("_", textColor);
				}
				if (start)
				{
					myTextures["bg"].render(0, 0);
					myTextures["btn1"].render(650, 266);
					myTextures["btn2"].render(650, 430);
				}
				else {
					myTextures["bg2"].render(0, 0);
					FindWord.render(404, 0);
					if (copy.size() != 0) {
						if (mouseY >= 0 && mouseY < display_number) myTextures["itemBtn"].render(0, 50 * mouseY + 204);
						for (int i = 0; i < min(10, copy.size() - cur); i++)
						{
							if (copy[i + cur].name != "")
							{
								item.load(copy[i + cur].name, textColor);
								item.render(0, 50 * i + 192);
							}
							else break;
						}
					}
					if (choose.name != "") {
						item.load(choose.name, textColor);
						item.render(350, 192);
						for (int i = 0; i < ChosenWord.size()-1; i++) {
							content.specialLoad(ChosenWord[i], textColor);
							content.render(350, 242 + i * 20);
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
