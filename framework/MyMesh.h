#ifndef _MY_CODE_MESH_H
#define _MY_CODE_MESH_H

#include <string>
#include <vector>
#include <memory>

namespace rapidxml
{
	template<typename T>
	class xml_node;
}

namespace MyCode
{
	class Attribute;
	class IndexData;
	struct MeshData;

	class Mesh
	{
	public:
		Mesh(std::string meshPath);
		~Mesh();

		void Render();
	private:
		typedef std::unique_ptr<std::vector<Attribute>> AttributeVectorPtr;
		typedef std::unique_ptr<std::vector<IndexData>> IndexDataVectorPtr;

		std::vector<char> ReadFileData(const std::string& filePath);
		void ParseXMLData(std::vector<char>& data);
		AttributeVectorPtr ParseXMLDataForAttributes(rapidxml::xml_node<char>* rootNode);
		IndexDataVectorPtr ParseXMLDataForIndices(rapidxml::xml_node<char>* rootNode);
		void ParseXMLDataForArrays(rapidxml::xml_node<char>* rootNode);

		void InitMeshData(AttributeVectorPtr attributes, IndexDataVectorPtr indexes);

		void SetupAttributes(AttributeVectorPtr& attributes);
		void SetupIndices(IndexDataVectorPtr& indexes);

		template<typename T>
		std::pair<std::vector<size_t>, size_t> GetBufferOffsets(std::unique_ptr<std::vector<T>>& values)
		{
			const size_t ALIGNMENT_BOUNDARY = 16;

			std::vector<size_t> offsets;
			size_t attributesBufferSize = 0;
			for (auto& value : *values)
			{
				attributesBufferSize = (attributesBufferSize % ALIGNMENT_BOUNDARY) ?
					(attributesBufferSize + (ALIGNMENT_BOUNDARY - attributesBufferSize % ALIGNMENT_BOUNDARY)) : attributesBufferSize;
				offsets.push_back(attributesBufferSize);
				attributesBufferSize += value.GetSizeInBytes();
			}

			return std::pair<std::vector<size_t>, size_t>(offsets, attributesBufferSize);
		}

		std::string mMeshFilePath;
		std::unique_ptr<MeshData> mMeshData;
	};
}

#endif _MY_CODE_MESH_H