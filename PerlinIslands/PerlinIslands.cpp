#ifdef _WIN64
	#define GMS2EXPORT extern "C" __declspec(dllexport)
#else
	#define GMS2EXPORT extern "C"
#endif

#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)

#include <cstring>
#include <string>

using namespace std;
char* getGMSBuffAddress(char* _GMSBuffPtrStr) {
    /*
		@description    Converts a GMS buffer address string to a usable pointer in C++.
		@params         {char*} _GMSBuffPtrStr - The ptr to a GMS buffer as a string.
		@return         {char*} The pointer to the buffer. Now functions like memcpy will work.
	*/
	size_t GMSBuffLongPointer = stoull(_GMSBuffPtrStr, NULL, 16);
	return (char*)GMSBuffLongPointer;
}

GMS2EXPORT char* ext_test_String(char* _myArgument0) {
	return (char*)_myArgument0;
}

GMS2EXPORT double ext_test_Double(double _myArgument0) {
	return (double)_myArgument0;
}

GMS2EXPORT double ext_test_Buffer(char* _GMSBuffPtrStr) {
	char* _GMSBuffer = getGMSBuffAddress(_GMSBuffPtrStr);

	unsigned int writeOffset = 0;
	unsigned short int myShort = 1337;
	unsigned int myUInt = 694201337;

	memcpy(&_GMSBuffer[writeOffset], &myShort, sizeof(myShort));
	writeOffset += sizeof(myShort);
	memcpy(&_GMSBuffer[writeOffset], &myUInt, sizeof(myUInt));

	return 1;
}

static int SEED = 0;

static int perlin_hash[] = {
  151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225,
  140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148,
  247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32,
  57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175,
  74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122,
  60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54,
  65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169,
  200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64,
  52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212,
  207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213,
  119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
  129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104,
  218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241,
  81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157,
  184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93,
  222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
};

float lerp(int a, int b, float s) {
	return a + s * (b - a);
}

float lerp_f(float a, float b, float s) {
	return a + s * (b - a);
}

float smooth_interpolation(int a, int b, float step) {
	return lerp(a, b, step * step * (3 - 2 * step));
}

float smooth_interpolation_f(float a, float b, float step) {
	return lerp_f(a, b, step * step * (3 - 2 * step));
}

int noise2(int x, int y) {
	int temp = perlin_hash[(y + SEED) % 256];
	return perlin_hash[(temp + x) % 256];
}

float noise(float x, float y) {
	int x_int = (int)x;
	int y_int = (int)y;
	float frac_x = x - x_int;
	float frac_y = y - y_int;
	int s = noise2(x_int, y_int);
	int t = noise2(x_int + 1, y_int);
	int u = noise2(x_int, y_int + 1);
	int v = noise2(x_int + 1, y_int + 1);
	float low = smooth_interpolation(s, t, frac_x);
	float high = smooth_interpolation(u, v, frac_x);
	return smooth_interpolation_f(low, high, frac_y);
}

float perlin(int x, int y, float frequency, int depth) {
	float xa = x * frequency;
	float ya = y * frequency;
	float amp = 1.0;
	float fin = 0;
	float div = 0.0;

	for (int i = 0; i < depth; i++) {
		div += 256 * amp;
		fin += noise(xa, ya) * amp;
		amp /= 2;
		xa *= 2;
		ya *= 2;
	}

	return fin / div;
}

static unsigned char tile_lookup[] = {
	 0,  0,  1,  1,  0,  0,  1,  1,  2,  2,  3,  4,  2,  2,  3,  4,  5,  5,  6,  6,  5,  5,  7,  7,  8,  8,  9, 10,  8,  8, 11, 12,
	 0,  0,  1,  1,  0,  0,  1,  1,  2,  2,  3,  4,  2,  2,  3,  4,  5,  5,  6,  6,  5,  5,  7,  7,  8,  8,  9, 10,  8,  8, 11, 12,
	13, 13, 14, 14, 13, 13, 14, 14, 15, 15, 16, 17, 15, 15, 16, 17, 18, 18, 19, 19, 18, 18, 20, 20, 21, 21, 22, 23, 21, 21, 24, 25,
	13, 13, 14, 14, 13, 13, 14, 14, 26, 26, 27, 28, 26, 26, 27, 28, 18, 18, 19, 19, 18, 18, 20, 20, 29, 29, 30, 31, 29, 29, 32, 33,
	 0,  0,  1,  1,  0,  0,  1,  1,  2,  2,  3,  4,  2,  2,  3,  4,  5,  5,  6,  6,  5,  5,  7,  7,  8,  8,  9, 10,  8,  8, 11, 12,
	 0,  0,  1,  1,  0,  0,  1,  1,  2,  2,  3,  4,  2,  2,  3,  4,  5,  5,  6,  6,  5,  5,  7,  7,  8,  8,  9, 10,  8,  8, 11, 12,
	13, 13, 14, 14, 13, 13, 14, 14, 15, 15, 16, 17, 15, 15, 16, 17, 34, 34, 35, 35, 34, 34, 36, 36, 37, 37, 38, 39, 37, 37, 40, 41,
	13, 13, 14, 14, 13, 13, 14, 14, 26, 26, 27, 28, 26, 26, 27, 28, 34, 34, 35, 35, 34, 34, 36, 36, 42, 42, 43, 44, 42, 42, 45, 46
};

GMS2EXPORT double ext_create_island(char* _GMSBuffPtrStr, double g_size, double random_seed) {
	// Set up a buffer channel to send over the data
	char* _GMSBuffer = getGMSBuffAddress(_GMSBuffPtrStr);
	unsigned int writeOffset = 0;

	// Prepare variables
	int x, y;
	int grid_size = (int)g_size;
	SEED = (int)random_seed;

	// Create a float grid of noise
	auto grid = new float[grid_size * grid_size];
	for (y = 0; y < grid_size; y++) {
		for (x = 0; x < grid_size; x++) {
			grid[x + grid_size * y] = perlin(x, y, 0.05f, 4);
		}
	}

	// Convert the float grid into terrain IDs (eg. water, sand, grass, rocks)
	int grid_center = grid_size / 2;
	float distance_from_center;
	double value_change, new_value;
	auto terrainID = new unsigned char[grid_size * grid_size];
	for (y = 0; y < grid_size; y++) {
		for (x = 0; x < grid_size; x++) {
			distance_from_center = max(abs(grid_center - x), abs(grid_center - y)) / (float)grid_center;
			value_change = 0.4 - pow(distance_from_center, 2);
			new_value = min(max(grid[x + y * grid_size] + value_change, 0), 1);

			if (new_value < 0.47) {	terrainID[x + y * grid_size] = 0; }
			else if (new_value < 0.54) { terrainID[x + y * grid_size] = 1; }
			else if (new_value < 0.98) { terrainID[x + y * grid_size] = 2; }
			else { terrainID[x + y * grid_size] = 3; }
			// memcpy(&_GMSBuffer[writeOffset], &tile_value, sizeof(tile_value));
			// writeOffset += sizeof(tile_value);
		}
	}

	// Using terrain IDs - for each cell create an automatic Tile ID for mapping
	// If the cell is on the edge, treat the edges as the same tile
	// Reference for tile corners: topleft: 1, topright: 2, bottomleft: 4, bottomright: 8
	unsigned char tileID;
	unsigned char tileSum = 0;
	unsigned char tileNum = 0;
	unsigned char topLeft, topRight, bottomLeft, bottomRight, top, left, right, bottom;
	for (y = 0; y < grid_size; y++) {
		for (x = 0; x < grid_size; x++) {
			// Base tileID based on terrainID
			if (terrainID[x + y * grid_size] == 0) {
				tileID = 8 + rand() % 8;
				memcpy(&_GMSBuffer[writeOffset], &tileID, sizeof(tileID));
				writeOffset += sizeof(tileID);
				continue;
			}

			top = terrainID[x + (y - 1) * grid_size];
			left = terrainID[x - 1 + (y) * grid_size];
			right = terrainID[x + 1 + (y) * grid_size];
			bottom = terrainID[x + (y + 1) * grid_size];

			topLeft = terrainID[x - 1 + (y - 1) * grid_size];
			topRight = terrainID[x + 1 + (y - 1) * grid_size];
			bottomLeft = terrainID[x - 1 + (y + 1) * grid_size];
			bottomRight = terrainID[x + 1 + (y + 1) * grid_size];

			tileSum = (
				(topLeft >= terrainID[x + y * grid_size] ? 1 : 0) +
				(top >= terrainID[x + y * grid_size] ? 2 : 0) +
				(topRight >= terrainID[x + y * grid_size] ? 4 : 0) +
				(left >= terrainID[x + y * grid_size] ? 8 : 0) +
				(right >= terrainID[x + y * grid_size] ? 16 : 0) +
				(bottomLeft >= terrainID[x + y * grid_size] ? 32 : 0) +
				(bottom >= terrainID[x + y * grid_size] ? 64 : 0) +
				(bottomRight >= terrainID[x + y * grid_size] ? 128 : 0)
			);
			
			if (tileSum == 255) {
				tileID = 32 + (terrainID[x + y * grid_size] - 1) * 64 + rand() % 16;
			} else {
				tileNum = tile_lookup[tileSum];
				tileID = 48 + (terrainID[x + y * grid_size] - 1) * 64 + tileNum;
			}
			memcpy(&_GMSBuffer[writeOffset], &tileID, sizeof(tileID));
			writeOffset += sizeof(tileID);
		}
	}

	delete[] grid;
	delete[] terrainID;

	return 1;
}