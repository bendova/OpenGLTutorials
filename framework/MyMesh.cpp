#include "MyMesh.h"

#include <fstream>
#include <stdexcept>
#include <map>
#include <streambuf>
#include <sstream>
#include <utility>
#include <type_traits>
#include "../framework/framework.h"
#include "../framework/rapidxml.hpp"
#include "../framework/rapidxml_helpers.h"

#define PARSE_ASSERT(cond, message) if(!cond) { throw new std::runtime_error(message); }

namespace MyCode
{
	namespace
	{
		void OnParseAttributeError(const rapidxml::xml_attribute<>& attribute, const std::string& msg)
		{
			std::string attributeName = rapidxml::make_string(attribute);
			throw new std::runtime_error("Attribute " + attributeName + " " + msg);
		}
	}

	union AttributeData
	{
		float floatValue;
		GLushort ushortValue;
	};

	struct AttributeType
	{
		const char* mDescription;
		GLenum mGLType;
		int mBytesCount;
		bool mNormalized;
		void(*ParseArrayData) (std::vector<AttributeData>& outAttributeData, std::stringstream& inData);
		void(*WriteToBuffer) (GLenum glBuffer, std::vector<AttributeData>& attributeData, size_t offset);
	};

	void ParseFloatsArrayData(std::vector<AttributeData>& outAttributeData, std::stringstream& data)
	{
		data.seekg(0, std::ios_base::beg);
		std::skipws(data);
		while ((data.eof() == false) && data.good())
		{
			AttributeData attributeData;
			data >> attributeData.floatValue >> std::ws;

			if (data.fail())
			{
				throw new std::runtime_error("ParseFloatsArrayData() Error while parsing float array.");
			}

			outAttributeData.push_back(attributeData);
		}
	}

	void ParseUShortArrayData(std::vector<AttributeData>& outAttributeData, std::stringstream& data)
	{
		data.seekg(0, std::ios_base::beg);
		std::skipws(data);
		while ((data.eof() == false) && data.good())
		{
			AttributeData attributeData;
			data >> attributeData.ushortValue >> std::ws;

			if (data.fail())
			{
				throw new std::runtime_error("ParseFloatsArrayData() Error while parsing data array.");
			}

			outAttributeData.push_back(attributeData);
		}
	}

	void WriteFlotsArrayToBuffer(GLenum glBuffer, std::vector<AttributeData>& attributeData, size_t offset)
	{
		std::vector<float> attributeDataCopy;
		attributeDataCopy.reserve(attributeData.size());
		for (unsigned i = 0; i < attributeData.size(); ++i)
		{
			attributeDataCopy.push_back(attributeData[i].floatValue);
		}
		glBufferSubData(glBuffer, offset, attributeDataCopy.size() * sizeof(float), &attributeDataCopy[0]);
	}

	void WriteUShortsArrayToBuffer(GLenum glBuffer, std::vector<AttributeData>& attributeData, size_t offset)
	{
		std::vector<GLushort> attributeDataCopy;
		attributeDataCopy.reserve(attributeData.size());
		for (unsigned i = 0; i < attributeData.size(); ++i)
		{
			attributeDataCopy.push_back(attributeData[i].ushortValue);
		}
		glBufferSubData(glBuffer, offset, attributeDataCopy.size() * sizeof(GLushort), &attributeDataCopy[0]);
	}

	std::map<const std::string, const AttributeType> gAttributeTypeMap =
	{
		{ "float", { "float", GL_FLOAT, sizeof(GLfloat), false, ParseFloatsArrayData, WriteFlotsArrayToBuffer } },
		{ "ushort", { "ushort", GL_UNSIGNED_SHORT, sizeof(GLushort), false, ParseUShortArrayData, WriteUShortsArrayToBuffer } }
	};

	class Attribute
	{
	public:
		explicit Attribute(const rapidxml::xml_node<>& element)
		{
			ParseXMLForAttributes(element);
			ParseXMLForData(element);
		}
		
		void FillBoundBufferObject(size_t offset)
		{
			mType->WriteToBuffer(GL_ARRAY_BUFFER, mAttributeData, offset);
		}

		void SetupAttributeArray(size_t offset)
		{
			glEnableVertexAttribArray(mIndex);
			glVertexAttribPointer(mIndex, mSize, mType->mGLType, mType->mNormalized ? GL_TRUE : GL_FALSE, 0, (void*)offset);
		}

		size_t GetSizeInBytes()
		{
			return mAttributeData.size() * mType->mBytesCount;
		}

		int GetIndex()
		{
			return mIndex;
		}

	private:
		void ParseXMLForAttributes(const rapidxml::xml_node<>& element)
		{
			mIndex = rapidxml::get_attrib_int(element, "index", OnParseAttributeError);
			if ((mIndex >= 0 && mIndex < 16) == false)
			{
				throw new std::runtime_error("Attribute::Attribute() mIndex must be in [0, 15]. mIndex is now " + mIndex);
			}

			mSize = rapidxml::get_attrib_int(element, "size", OnParseAttributeError);
			if ((mSize > 0 && mSize < 5) == false)
			{
				throw new std::runtime_error("Attribute::Attribute() mSize must be in [0, 4]. mSize is now " + mSize);
			}

			mType = &gAttributeTypeMap.at(rapidxml::get_attrib_string(element, "type"));
		}

		void ParseXMLForData(const rapidxml::xml_node<>& element)
		{
			std::stringstream stringStream;
			for (const rapidxml::xml_node<char>* node = element.first_node();
				node;
				node = node->next_sibling())
			{
				stringStream.write(node->value(), node->value_size());
			}
			stringStream.flush();
			mType->ParseArrayData(mAttributeData, stringStream);
		}

		int mIndex;
		int mSize;
		const AttributeType* mType;
		std::vector<AttributeData> mAttributeData;
	};

	class IndexData
	{
	public:
		explicit IndexData(const rapidxml::xml_node<>& element)
		{
			ParseXMLForAttributes(element);
			ParseXMLForData(element);
		}

		void FillBoundBufferObject(size_t offset)
		{
			mType->WriteToBuffer(GL_ELEMENT_ARRAY_BUFFER, mAttributeData, offset);
		}

		size_t GetSizeInBytes()
		{
			return mAttributeData.size() * mType->mBytesCount;
		}

		size_t GetDataSize()
		{
			return mAttributeData.size();
		}

		GLenum GetGLType()
		{
			return mType->mGLType;
		}

	private:
		void ParseXMLForAttributes(const rapidxml::xml_node<>& element)
		{
			mType = &gAttributeTypeMap.at(rapidxml::get_attrib_string(element, "type"));
		}

		void ParseXMLForData(const rapidxml::xml_node<>& element)
		{
			std::stringstream stringStream;
			for (const rapidxml::xml_node<char>* node = element.first_node();
				node;
				node = node->next_sibling())
			{
				stringStream.write(node->value(), node->value_size());
			}
			stringStream.flush();
			mType->ParseArrayData(mAttributeData, stringStream);
		}

		const AttributeType* mType;
		std::vector<AttributeData> mAttributeData;
	};

	std::map<std::string, GLenum> gPrimitiveTypes =
	{
		{ "triangles", GL_TRIANGLES },
		{ "tri-strip", GL_TRIANGLE_STRIP },
		{ "tri-fan", GL_TRIANGLE_FAN },
		{ "lines", GL_LINES },
		{ "line-strip", GL_LINE_STRIP },
		{ "line-loop", GL_LINE_LOOP },
		{ "points", GL_POINTS },
	};

	struct RenderCommand
	{
		GLuint mStartIndex;
		GLuint mCount;
		GLenum mDataType;
		GLenum mPrimitiveType;
		bool mIsIndexed;

		RenderCommand()
			: mStartIndex(0)
			, mCount(0)
			, mDataType(GL_NO_ERROR)
			, mPrimitiveType(GL_NO_ERROR)
			, mIsIndexed(false)
		{}

		void Render()
		{
			if (mIsIndexed)
			{
				glDrawElements(mPrimitiveType, mCount, mDataType, (void*)mStartIndex);
			}
			else
			{
				glDrawArrays(mPrimitiveType, mStartIndex, mCount);
			}
		}
	};

	RenderCommand ParseXMLForRenderCommand(const rapidxml::xml_node<> &cmdElem)
	{
		RenderCommand command;

		const std::string commandName = rapidxml::get_attrib_string(cmdElem, "cmd");
		command.mPrimitiveType = gPrimitiveTypes.at(commandName);

		const std::string elementName = rapidxml::make_string_name(cmdElem);
		if (elementName == "indices")
		{
			command.mIsIndexed = true;
		}
		else if (elementName == "arrays")
		{
			command.mStartIndex = static_cast<GLint>(rapidxml::get_attrib_int(cmdElem, "start", OnParseAttributeError));
			if (command.mStartIndex < 0)
			{
				throw new std::runtime_error("ParseXMLForRenderCommand() 'start' attribute from " + elementName + " must be >= 0");
			}

			command.mCount = static_cast<GLint>(rapidxml::get_attrib_int(cmdElem, "count", OnParseAttributeError));
			if (command.mCount <= 0)
			{
				throw new std::runtime_error("ParseXMLForRenderCommand() 'count' attribute from " + elementName + " must be > 0");
			}
		}
		else
		{
			throw std::runtime_error("Bad command element " + elementName + ". Must be 'indices' or 'arrays'.");
		}

		return command;
	}

	struct MeshData
	{
	public:
		MeshData()
			: mVertexArrayObjectID(GL_NONE)
			, mVertexBufferID(GL_NONE)
			, mIndexBufferID(GL_NONE)
		{}

		GLuint mVertexArrayObjectID; 
		GLuint mVertexBufferID;
		GLuint mIndexBufferID;
		std::vector<RenderCommand> mRenderCommands;
		NamedVaoVector mNamedVAOsVector;
		NamedVaoMap mNamedVAOsMap;
		std::pair<std::vector<size_t>, size_t> mAttributeBufferOffsetsData;
	};

	Mesh::Mesh(std::string meshPath)
		: mMeshData(new MeshData())
	{
		mMeshFilePath = Framework::FindFileOrThrow(meshPath);
		std::vector<char> data = ReadFileData(mMeshFilePath);
		ParseXMLData(data);
	}

	Mesh::~Mesh()
	{

	}

	std::vector<char> Mesh::ReadFileData(const std::string& filePath)
	{
		std::vector<char> data;

		std::ifstream file(filePath, std::ifstream::in);
		if (file.is_open())
		{
			data.reserve(2 << 20);
			data.insert(data.end(), std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
			data.push_back('\0');

			file.close();
		}
		else
		{
			throw new std::runtime_error("Mesh::Mesh() Could not open file: " + filePath);
		}

		return data;
	}

	void Mesh::ParseXMLData(std::vector<char>& data)
	{
		rapidxml::xml_document<char> document;

		try
		{
			document.parse<0>(&data[0]);
		}
		catch (rapidxml::parse_error& e)
		{
			printf("Mesh::ParseXMLData() XML error: %s, at %s", e.what(), e.where<char>());
			throw;
		}

		rapidxml::xml_node<char>* rootNode = document.first_node("mesh");
		PARSE_ASSERT(rootNode, "Could not find root 'mesh' node in " + mMeshFilePath);

		auto attributes = ParseXMLDataForAttributes(rootNode);
		auto indices = ParseXMLDataForIndices(rootNode);
		ParseXMLDataForArrays(rootNode);
		ParseXMLDataForNamedVAOs(rootNode);
		InitMeshData(std::forward<AttributeVector>(attributes), 
			std::forward<IndexDataVector>(indices));
	}

	Mesh::AttributeVector Mesh::ParseXMLDataForAttributes(rapidxml::xml_node<char>* rootNode)
	{
		rapidxml::xml_node<char>* node = rootNode->first_node("attribute");
		PARSE_ASSERT(rootNode, "The mesh should have at least one 'attribute' node in " + mMeshFilePath);

		AttributeVector attributes = std::vector<Attribute>();
		for (; node && (rapidxml::make_string_name(*node) == "attribute");
			node = rapidxml::next_element(node))
		{
			attributes.push_back(Attribute(*node));
		}

		return attributes;
	}

	Mesh::IndexDataVector Mesh::ParseXMLDataForIndices(rapidxml::xml_node<char>* rootNode)
	{
		rapidxml::xml_node<char>* node = rootNode->first_node("indices");

		IndexDataVector indices = std::vector<IndexData>();
		for (;
			node && (rapidxml::make_string_name(*node) == "indices");
			node = rapidxml::next_element(node))
		{
			mMeshData->mRenderCommands.push_back(ParseXMLForRenderCommand(*node));
			indices.push_back(IndexData(*node));
		}
		return indices;
	}
	
	void Mesh::ParseXMLDataForArrays(rapidxml::xml_node<char>* rootNode)
	{
		rapidxml::xml_node<char>* node = rootNode->first_node("arrays");
		for (;
			node && (rapidxml::make_string_name(*node) == "arrays");
			node = rapidxml::next_element(node))
		{
			mMeshData->mRenderCommands.push_back(ParseXMLForRenderCommand(*node));
		}
	}

	void Mesh::ParseXMLDataForNamedVAOs(rapidxml::xml_node<char>* rootNode)
	{
		rapidxml::xml_node<char>* node = rootNode->first_node("vao");
		for (;
			node && (make_string_name(*node) == "vao");
			node = rapidxml::next_element(node))
		{
			mMeshData->mNamedVAOsVector.push_back(ParseXMLDataForVAO(node));
		}
	}

	NamedVao Mesh::ParseXMLDataForVAO(rapidxml::xml_node<char>* vaoElem)
	{
		std::string vaoName = rapidxml::get_attrib_string(*vaoElem, "name");
		std::vector<GLuint> sourceAttribIndexes;
		for (const rapidxml::xml_node<> *pSource = vaoElem->first_node("source");
			pSource;
			pSource = pSource->next_sibling("source"))
		{
			sourceAttribIndexes.push_back(rapidxml::get_attrib_int(*pSource, "attrib", OnParseAttributeError));
		}
		return std::make_pair<std::string, std::vector<GLuint>>(std::forward<std::string>(vaoName), 
			std::forward<std::vector<GLuint>>(sourceAttribIndexes));
	}

	void Mesh::InitMeshData(AttributeVector attributes, IndexDataVector indices)
	{
		glGenVertexArrays(1, &mMeshData->mVertexArrayObjectID);
		glBindVertexArray(mMeshData->mVertexArrayObjectID);

		SetupAttributes(attributes);

		glBindVertexArray(GL_NONE);

		SetupIndices(indices);
		SetupNamedVAOsMap(attributes);
	}

	void Mesh::SetupAttributes(AttributeVector& attributes)
	{
		mMeshData->mAttributeBufferOffsetsData = GetBufferOffsets(attributes);
		auto& attributesOffsets = mMeshData->mAttributeBufferOffsetsData.first;
		size_t attributesBufferSize = mMeshData->mAttributeBufferOffsetsData.second;

		glGenBuffers(1, &mMeshData->mVertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, mMeshData->mVertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, attributesBufferSize, NULL, GL_STATIC_DRAW);

		for (unsigned i = 0; i < attributes.size(); ++i)
		{
			Attribute& attribute = attributes[i];
			attribute.FillBoundBufferObject(attributesOffsets[i]);
			attribute.SetupAttributeArray(attributesOffsets[i]);
		}
	}

	void Mesh::SetupIndices(IndexDataVector& indices)
	{
		auto bufferOffsetsPair = GetBufferOffsets(indices);
		auto& indicesOffsets = bufferOffsetsPair.first;
		size_t indicesBufferSize = bufferOffsetsPair.second;

		if (indicesBufferSize > 0)
		{
			glBindVertexArray(mMeshData->mVertexArrayObjectID);
			glGenBuffers(1, &mMeshData->mIndexBufferID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mMeshData->mIndexBufferID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesBufferSize, NULL, GL_STATIC_DRAW);

			for (unsigned i = 0; i < indices.size(); ++i)
			{
				indices[i].FillBoundBufferObject(indicesOffsets[i]);
			}

			auto& renderCommands = mMeshData->mRenderCommands;
			for (unsigned i = 0; i < renderCommands.size(); ++i)
			{
				auto& renderCommand = renderCommands[i];
				if (renderCommand.mIsIndexed)
				{
					auto& indexData = indices[i];

					renderCommand.mStartIndex = static_cast<GLuint>(indicesOffsets[i]);
					renderCommand.mCount = static_cast<GLuint>(indexData.GetDataSize());
					renderCommand.mDataType = indexData.GetGLType();
				}
			}

			glBindVertexArray(GL_NONE);
		}
	}

	void Mesh::SetupNamedVAOsMap(AttributeVector& attributes)
	{
		for (auto& namedVao : mMeshData->mNamedVAOsVector)
		{
			GLuint vao = -1;
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			auto& attributesOffsets = mMeshData->mAttributeBufferOffsetsData.first;
			for (size_t iAttribIx = 0; iAttribIx < namedVao.second.size(); iAttribIx++)
			{
				GLuint iAttrib = namedVao.second[iAttribIx];
				int iAttribOffset = -1;
				for (size_t iCount = 0; iCount < attributes.size(); ++iCount)
				{
					if (attributes[iCount].GetIndex() == iAttrib)
					{
						iAttribOffset = iCount;
						break;
					}
				}

				Attribute &attrib = attributes[iAttribOffset];
				attrib.SetupAttributeArray(attributesOffsets[iAttribOffset]);
			}

			mMeshData->mNamedVAOsMap[namedVao.first] = vao;

			glBindVertexArray(vao);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mMeshData->mIndexBufferID);
		}
		glBindVertexArray(GL_NONE);
	}

	void Mesh::Render() const
	{
		if (mMeshData->mVertexArrayObjectID != GL_NONE)
		{
			glBindVertexArray(mMeshData->mVertexArrayObjectID);

			for (auto& renderCommand : mMeshData->mRenderCommands)
			{
				renderCommand.Render();
			}

			glBindVertexArray(GL_NONE);
		}
	}

	void Mesh::Render(const std::string& vaoName) const
	{
		auto iter = mMeshData->mNamedVAOsMap.find(vaoName);
		if (iter != mMeshData->mNamedVAOsMap.end())
		{
			auto vaoIndex = iter->second;
			glBindVertexArray(vaoIndex);
			for (auto& renderCommand : mMeshData->mRenderCommands)
			{
				renderCommand.Render();
			}
			glBindVertexArray(GL_NONE);
		}
	}
}