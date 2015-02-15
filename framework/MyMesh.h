#ifndef _MY_CODE_MESH_H
#define _MY_CODE_MESH_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <glload/gl_3_3.h>

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

	typedef std::pair<std::string, std::vector<GLuint>> NamedVao;
	typedef std::vector<NamedVao> NamedVaoVector;
	typedef std::map<std::string, GLuint> NamedVaoMap;

	class Mesh
	{
	public:
		Mesh(std::string meshPath);
		~Mesh();

		void Render() const;
		void Render(const std::string& vaoName) const;
	private:
		typedef std::vector<Attribute> AttributeVector;
		typedef std::vector<IndexData> IndexDataVector;

		std::vector<char> ReadFileData(const std::string& filePath);
		void ParseXMLData(std::vector<char>& data);
		AttributeVector ParseXMLDataForAttributes(rapidxml::xml_node<char>* rootNode);
		IndexDataVector ParseXMLDataForIndices(rapidxml::xml_node<char>* rootNode); 
		void ParseXMLDataForArrays(rapidxml::xml_node<char>* rootNode);
		void ParseXMLDataForNamedVAOs(rapidxml::xml_node<char>* rootNode);
		NamedVao ParseXMLDataForVAO(rapidxml::xml_node<char>* rootNode);

		void InitMeshData(AttributeVector attributes, IndexDataVector indexes);

		void SetupAttributes(AttributeVector& attributes);
		void SetupIndices(IndexDataVector& indexes);
		void SetupNamedVAOsMap(AttributeVector& attributes);

		template<typename T>
		std::pair<std::vector<size_t>, size_t> GetBufferOffsets(std::vector<T>& values)
		{
			const size_t ALIGNMENT_BOUNDARY = 16;

			std::vector<size_t> offsets;
			size_t attributesBufferSize = 0;
			for (auto& value : values)
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