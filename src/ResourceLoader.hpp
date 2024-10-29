//------------------------------------------------------------------------------
// File: ResourceLoader.hpp
// Author: Chris Redwood
// Created: 2024-10-29
// License: MIT License
//------------------------------------------------------------------------------

#pragma once

#if 0

#include <map>
#include <string>

namespace shmup {

class Resource : public std::enable_shared_from_this<Resource> {
public:
  Resource(const char* filepath);

  ~Resource();

  const std::string& filepath() const;


private:
  std::string m_filepath;
};

/**
 * @brief ResourceLoader: 
 * resources.meta 이진 텍스트 파일을 읽어서 resources 아래에 있는 리소스 파일 로드
 * 
 */
class ResourceLoader {
public:
  ResourceLoader() = default;
  
  ~ResourceLoader() = default;

  /// @brief 리소스 메타 파일을 읽고 명시된 응용 프로그램에 사용되는 모든 리소스를 읽음
  /// @param metaFilepath 
  /// @return true if there was no error, otherwise false
  bool readAllResources(const char* metaFilepath);

  /// @brief 하나의 리소스 파일을 읽음
  /// @param filepath 
  /// @return 
  bool readResource(const char* filepath);

private:
  std::map<std::string, std::shared_ptr<Resource>> m_resourceMap;
};

}

#endif