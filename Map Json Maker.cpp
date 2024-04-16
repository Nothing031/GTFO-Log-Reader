#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

#include <windows.h>
#include <vector>

#include <nlohmann/json.hpp>
#include <fstream>

using namespace std;
using namespace cv;
using json = nlohmann::json;

class Position {
    public:
        int x;
        int y;
};
Position posList[100];
int clickCount;

void PrintClickCount() {
    cout << "\r" << clickCount << "             ";
}
void onMouseClick(int event, int x, int y, int flags, void* userdata) {
    if (event == EVENT_LBUTTONDOWN) {
        Position pos;
        pos.x = x;
        pos.y = y;
        posList[clickCount] = pos;
        clickCount++;
        PrintClickCount();
    }
}
int main() {
    // set window allways on top
    HWND hwnd = GetConsoleWindow();
    if (hwnd == NULL) {
        return 1;
    }
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    RECT rect;
    GetWindowRect(hwnd, &rect);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int titlebarHeight = GetSystemMetrics(SM_CYCAPTION);
    RECT workArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
    int workAreaHeight = workArea.bottom - workArea.top;
    int taskbarHeight = screenHeight - workAreaHeight;

    int resizeWindowWidth = screenWidth;
    int resizeWindowHeight = workAreaHeight - titlebarHeight;

    double WH_DisplayRatio = (double)resizeWindowWidth / resizeWindowHeight;

    string filePath;
    string name;
    size_t pos;

    while (true) {
        MoveWindow(hwnd, rect.left, rect.top, 800, 300, TRUE);
        // get input

        cout << "drag and drop Map image file and press enter" << endl;
        getline(cin, filePath);
        filePath.erase(remove(filePath.begin(), filePath.end(), '\"'), filePath.end());
        cout << filePath << endl;

        pos = filePath.find_last_of('.');
        name = filePath.substr(0, pos);
        name += ".json";

        ifstream file(filePath);
        if (!file.good()) { // if not exists
            cout << "file not exists" << endl;
            continue;
        }
        file.close();
        // initialize
        clickCount = 0;
        MoveWindow(hwnd, 0, 0, 500, 100, TRUE);

        Mat image = imread(filePath);

        int imageWidth = image.cols;
        int imageHeight = image.rows;
        double WH_ImageRatio = (double)imageWidth / imageHeight;
        int ResizeImageWidth = 0;
        int ResizeImageHeight = 0;

        if (WH_DisplayRatio < WH_ImageRatio) { // if width is longer
            ResizeImageWidth = resizeWindowWidth;
            ResizeImageHeight = resizeWindowWidth * WH_ImageRatio;
        }
        else {
            ResizeImageWidth = resizeWindowHeight * WH_ImageRatio;
            ResizeImageHeight = resizeWindowHeight;
        }

        namedWindow("image", WINDOW_NORMAL);
        resizeWindow("image", ResizeImageWidth  , ResizeImageHeight);

        imshow("image", image);
        setMouseCallback("image", onMouseClick);
        cout << "\n\n\n\n\n\n\n\n\n\n";
        cout << "\"esc\": done | \"backspace\": remove prev position" << endl;
        cout << "click this number" << endl;
        cout << 0;
        bool whileloop = true;
        while (whileloop) {
            int key = waitKey(1);
            switch (key) {
            case 27: {
                whileloop = false;
                break;
            }
            case 8: {
                if (clickCount > 0) {
                    clickCount--;
                    PrintClickCount();
                    break;
                }
            }
            }
        }
        destroyAllWindows();
        image.release();
        cout << "\r     " << endl;
    
        MoveWindow(hwnd, rect.left, rect.top, 800, 300, TRUE);

        // save json
        json data = json::array();

        for (int i = 0; i < clickCount; i++) {
            json obj = {
                {"Ri", i },
                {"Position",{
                    {"x", posList[i].x},
                    {"y", posList[i].y},
                }}
            };
            data.push_back(obj);
        }
        ofstream ofs(name);
        ofs << setw(4) << data << endl;
        ofs.close();
        cout << "done" << endl;
    }
    return 0;
}