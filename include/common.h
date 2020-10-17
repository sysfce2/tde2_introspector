#pragma once


#include <string>
#include <vector>
#include <fstream>
#include <array>
#include <cstdint>
#include <sstream>
#include <memory>
#include <functional>
#include <unordered_map>
#include <mutex>


namespace TDEngine2
{
#if TDE2_USE_NOEXCEPT
	#define TDE2_NOEXCEPT noexcept
#else 
	#define TDE2_NOEXCEPT 
#endif


	class SymTable;


	static struct TVersion
	{
		const uint32_t mMajor = 0;
		const uint32_t mMinor = 1;
	} ToolVersion;

	struct TIntrospectorOptions
	{
		static constexpr uint16_t mMaxNumOfThreads = 32;

		bool                      mIsValid;
		bool                      mIsTaggedOnlyModeEnabled = false;
		bool                      mIsLogOutputEnabled = true;

		std::vector<std::string>  mInputSources { "." };

		std::string               mCacheDirname = "./cache/";
		std::string               mCacheIndexFilename = "index.cache";

		std::string               mOutputDirname = ".";
		std::string               mOutputFilename = "metadata.h";

		uint16_t                  mCurrNumOfThreads = 1;

		static TIntrospectorOptions mInvalid;
	};

	TIntrospectorOptions ParseOptions(int argc, const char** argv) TDE2_NOEXCEPT;

	std::vector<std::string> GetHeaderFiles(const std::vector<std::string>& directories) TDE2_NOEXCEPT;
	
	void WriteOutput(const std::string& text) TDE2_NOEXCEPT;

	std::unique_ptr<SymTable> ProcessHeaderFile(const std::string& filename) TDE2_NOEXCEPT;


	const std::string GeneratedHeaderPrelude = R"(
/*!
	Autogenerated by tde2_introspector tool 
*/

#include <array>
#include <string>
#include <type_traits>

{0}


namespace Meta 
{

enum class MetaEntityType: uint8_t
{
	Enum,
	Class,
	Struct,
	Function,
	Method,
	Unknown
};


/*!
	\brief The method computes 32 bits hash based on an input string's value.
	The underlying algorithm's description can be found here
	http://www.cse.yorku.ca/~oz/hash.html

	\param[in] pStr An input string
	\param[in] hash The argument is used to store current hash value during a recursion

	\return 32 bits hash of the input string
*/

constexpr uint32_t ComputeHash(const char* pStr, uint32_t hash = 5381)
{
	return (*pStr != 0) ? ComputeHash(pStr + 1, ((hash << 5) + hash) + *pStr) : hash;
}


enum class TypeID : uint32_t 
{
	Invalid = 0x0
};


#define TYPEID(TypeName) static_cast<TypeID>(ComputeHash(#TypeName))


/*!
	\brief Use Type<TYPEID(...)>::Value to get type by its TypeID
*/

template <TypeID id> struct Type { };


/*
	\brief The section is auto generated code that contains all needed types, functcions and other
	infrastructure to provide correct work of meta-data
*/

template <typename TEnum>
struct EnumFieldInfo
{
	const TEnum       value;
	const std::string name;
};

template <typename TEnum>
struct EnumTrait
{
	static const bool         isOpaque = false;
	static const unsigned int elementsCount = 0;

	static const std::array<EnumFieldInfo<TEnum>, 0>& GetFields() { return {}; }
};


template <typename TClass>
struct ClassTrait
{
	static const std::string name;
	static constexpr TypeID  typeID = TypeID::Invalid;

	static const bool isInterface;
	static const bool isAbstract;
};


struct EnumInfo
{	
};


struct ClassInfo
{
};


struct TypeInfo
{
	TypeID         mID;
	MetaEntityType mType;
	std::string    mName;

	union
	{
		/// 
	}              mRawInfo;
};

	)";


	class IOutputStream
	{
		public:
			virtual ~IOutputStream() = default;

			virtual bool Open() = 0;
			virtual bool Close() = 0;

			virtual bool WriteString(const std::string& data) = 0;
		protected:
			IOutputStream() = default;
	};


	class FileOutputStream : public IOutputStream
	{
		public:
			FileOutputStream() = delete;
			explicit FileOutputStream(const std::string& filename);
			virtual ~FileOutputStream();

			bool Open() override;
			bool Close() override;

			bool WriteString(const std::string& data) override;
		private:
			std::string mFilename;

			std::ofstream mFileStream;
	};


	class StringUtils
	{
		public:
			static const inline std::string& GetEmptyStr()
			{
				static const std::string emptyStr{};
				return emptyStr;
			}

			static std::string ReplaceAll(const std::string& input, const std::string& what, const std::string& replacement);

			template <typename... TArgs>
			static std::string Format(const std::string& formatStr, TArgs&&... args)
			{
				constexpr uint32_t argsCount = sizeof...(args);

				std::array<std::string, argsCount> arguments;
				_convertToStringsArray<sizeof...(args), TArgs...>(arguments, std::forward<TArgs>(args)...);

				std::string formattedStr = formatStr;
				std::string currArgValue;
				std::string currArgPattern;

				currArgPattern.reserve(5);

				std::string::size_type pos = 0;

				/// \note replace the following patterns {i}
				for (uint32_t i = 0; i < argsCount; ++i)
				{
					currArgPattern = "{" + ToString<uint32_t>(i) + "}";
					currArgValue = arguments[i];

					while ((pos = formattedStr.find(currArgPattern)) != std::string::npos)
					{
						formattedStr.replace(pos, currArgPattern.length(), currArgValue);
					}
				}

				return formattedStr;
			}

			template <typename T>
			static std::string ToString(const T& arg)
			{
				std::ostringstream stream;
				stream << arg;
				return stream.str();
			}
	private:
		template <uint32_t size>
		static void _convertToStringsArray(std::array<std::string, size>& outArray) {}

		template <uint32_t size, typename Head, typename... Tail>
		static void _convertToStringsArray(std::array<std::string, size>& outArray, Head&& firstArg, Tail&&... rest)
		{
			outArray[size - 1 - sizeof...(Tail)] = ToString(std::forward<Head>(firstArg));
			_convertToStringsArray<size, Tail...>(outArray, std::forward<Tail>(rest)...);
		}
	};


	class DeferOperation
	{
		public:
			DeferOperation() = delete;
			DeferOperation(const std::function<void()>& action) :
				mAction(action)
			{
			}

			~DeferOperation()
			{
				if (mAction)
				{
					mAction();
				}
			}

		private:
			std::function<void()> mAction = nullptr;
	};

#define DEFER(...) DeferOperation deferOp(__VA_ARGS__)


	/*!
		\brief The method computes 32 bits hash based on an input string's value.
		The underlying algorithm's description can be found here
		http://www.cse.yorku.ca/~oz/hash.html

		\param[in] pStr An input string
		\param[in] hash The argument is used to store current hash value during a recursion

		\return 32 bits hash of the input string
	*/

	constexpr uint32_t ComputeHash(const char* pStr, uint32_t hash = 5381)
	{
		return (*pStr != 0) ? ComputeHash(pStr + 1, ((hash << 5) + hash) + *pStr) : hash;
	}


	class TCacheData
	{
		public:
			using TCacheIndexTable = std::unordered_map<std::string, std::string>;

		public:
			/*!
				\brief The function returns an index of cached symbols per file. The key of the table
				is a full header's path, the value is a name of corresponding cache file. The name is
				SHA-256 hashsum
			*/

			bool Load(const std::string& cacheSourceDirectory, const std::string& cacheFilename);

			/*!
				\brief The functions writes down current options into a main cache file
			*/

			bool Save(const std::string& cacheSourceDirectory, const std::string& cacheFilename);

			void Reset();

			void AddSymTableEntity(const std::string& filePath, const std::string& fileHash);

			bool Contains(const std::string& filePath, const std::string& fileHash) const;

			void SetInputHash(const std::string& hash);
			void SetSymTablesIndex(TCacheIndexTable&& table);

			const TCacheIndexTable& GetSymTablesIndex() const;
			const std::string& GetInputHash() const;
		private:
			mutable std::mutex mMutex;

			std::string mInputHash;

			TCacheIndexTable mSymTablesTable;
	};

	
	/*!
		\brief All file paths within the vector should have canonical form to prevent different hashes for representations of same files
	*/

	std::string GetHashFromInputFiles(const std::vector<std::string>& inputFiles);
	std::string GetHashFromFilePath(const std::string& value);
}