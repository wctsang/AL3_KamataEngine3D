#pragma once
#include <cstdint>
#include <vector>
#include "Vector3.h"

enum class MapChipType {
	kBlank,
	kBlock,
};

typedef struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
} MapChipData;

class MapChipField {
public:

	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	struct Rect {
		float left;
		float right;
		float bottom;
		float top;
	};

	void ResetMapChipData();

	void LoadMapChipCsv(const std::string& filePath);

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	IndexSet GetMapChipIndexSetByPosition(const Vector3& position);

	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

	uint32_t GetNumBlockVertical();
	uint32_t GetNumBlockHorizontal();

	private:

	//ブロックサイズ

	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;

	//ブロックの個数

	static inline const uint32_t kNumBlockVertical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	MapChipData mapChipData_;
};