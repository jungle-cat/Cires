/*M
 *
 * File Name     :  ImageMetaInfo.hpp
 *
 * Creation Date :  2013-4-13
 *
 * Last Modified :  2013-4-13
 *
 * Created By    :  Feng Weiguo (ustcrevolutionary@gmail.com)
 * 
 * Copyright     :  Any kinds use of the code is only legal with author's permission.
 *
 */

#ifndef __IMAGEMETAINFO_HPP__
#define __IMAGEMETAINFO_HPP__


#include "precomp.hpp"

class BaseInfo
{
	public:
		typedef unsigned int id_type;
		const static id_type nindex = static_cast<id_type>(-1);

		BaseInfo(id_type i = 0, const std::string& n = "")
			: id(i), name(n)
		{
		}

		BaseInfo(const std::string& n = "")
			: id(0), name(n)
		{
		}

	public:
		id_type     id;
		std::string name;
};

class ImageMetaInfo : public BaseInfo
{
	public:
		typedef BaseInfo::id_type id_type;
		const static id_type nindex = BaseInfo::nindex;

		ImageMetaInfo( id_type i = 0,
					   const std::string& n = "",
					   const cv::Mat& image = cv::Mat())
			: BaseInfo(i, n)
			, orginSize(image.size())
			, normSize(image.size())
		{
		}
		ImageMetaInfo( const std::string& name,
					   const cv::Mat& image = cv::Mat(),
					   const cv::Size& normedSize = cv::Size())
			: BaseInfo(name)
			, orginSize(image.size())
			, normSize(normedSize == cv::Size() ? image.size() : normedSize)
		{
		}

	public:
		cv::Size orginSize;
		cv::Size normSize;
};

template <typename MetaInfoType>
class ImageMetaInfoIndex
{
	public:
		typedef MetaInfoType                   node_type;
		typedef typename MetaInfoType::id_type id_type;

		static const id_type nindex = MetaInfoType::nindex;

		node_type getNodeByID(id_type id) const
		{
			std::string name;
			if (id < 0 || id >= nameStore.size()
					|| (name = nameStore[id]).empty())
				return node_type(nindex);
			return getNodeByName(name);
		}

		node_type getNodeByName(const std::string& name) const
		{
			auto it = nodeIndexer.find(name);
			if (it == nodeIndexer.end())
				return node_type(nindex);
			else
				return it->second;
		}

		id_type queryIDByName(const std::string& name) const
		{
			return getNodeByName(name).id;
		}

		id_type queryID(id_type id) const
		{
			return getNodeByID(id).id;
		}

		id_type insert(node_type& node)
		{
			id_type id = insert(const_cast<const node_type&>(node));
			node.id = id;
			return id;
		}

		id_type insert(const node_type& node)
		{
			auto it = nodeIndexer.find(node.name);
			if (it != nodeIndexer.end())
				return it->second.id;

			// get new index id
			id_type id;
			if (unUsedIds.empty()) {
				id = nameStore.size();
				nameStore.push_back(node.name);
			}
			else {
				id = unUsedIds.back();
				unUsedIds.pop_back();
				nameStore[id] = node.name;
			}

			// insert the node into index
			node_type copied_node = node;
			copied_node.id = id;
			nodeIndexer[node.name] = copied_node;
			return id;
		}

	private:
		std::unordered_map<std::string, node_type>  nodeIndexer;
		std::vector<std::string>                    nameStore;
		std::deque<id_type>                         unUsedIds;
};


#endif // __IMAGEMETAINFO_HPP__
