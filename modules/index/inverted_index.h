/*
 * inverted_index.h
 *
 *  Created on: 2014年3月31日
 *      Author: Feng
 */

#ifndef __INVERTED_INDEX_H__
#define __INVERTED_INDEX_H__

#include <common/common.h>


#define ORI_HIST_LEN 16
#define LOGS_HIST_LEN 16

namespace cires {


struct InvertedIndexNode
{
	typedef unsigned int word_type;
	typedef unsigned long long hash_type;

	id_type  image_id;
	id_type  keypoint_id;
	hash_type     binary;

	float  orientation;
	float logscale;

	InvertedIndexNode()
		: image_id(-1), keypoint_id(-1), binary(-1), orientation(-1), logscale(-1)
	{}

	InvertedIndexNode(unsigned int iid, unsigned int kid, float o, float s)
		: image_id(iid), keypoint_id(kid), binary(-1), orientation(o), logscale(s)
	{}
};


struct CandidateElemInfo
{
	std::vector<int> ori_hist;
	std::vector<int> logs_hist;

	CandidateElemInfo()
		: ori_hist(ORI_HIST_LEN, 0), logs_hist(LOGS_HIST_LEN, 0)
	{}
};



class InvertedIndexPostingList
{
	public:
		typedef std::deque<InvertedIndexNode>::const_iterator const_iterator;
		typedef std::deque<InvertedIndexNode>::iterator iterator;

		InvertedIndexPostingList()
			: weight(1)
		{}

		void add( const InvertedIndexNode& n )
		{
			storage.push_back(n);
		}
		tuple<const_iterator, const_iterator> collections() const
		{
			return make_tuple(storage.begin(), storage.end());
		}

	private:
		bool   activated;
		std::deque<InvertedIndexNode> storage;

	public:
		float  weight;
};


class InvertedIndex
{
	public:
		typedef InvertedIndexNode::word_type word_type;
		typedef InvertedIndexNode::hash_type hash_type;

		InvertedIndex( size_t voc_num = 0 )
		{}

		template <typename ...Args>
		void add(word_type wid, Args... args)
		{
			InvertedIndexNode n(std::forward<Args>(args)...);
			storage[wid].add(n);
		}

		template <typename Iterator>
		void add( id_type image_id, Iterator start, Iterator stop )
		{
		}

		template <typename Iterator>
		vector<tuple<id_type, float>> query(Iterator start, Iterator stop)
		{
			std::unordered_map<unsigned int, CandidateElemInfo> candidates;

			for ( auto it1 = start; it1 != stop; ++it1) {
				word_type w = it1->word_id;
				float s = it1->logscale;
				float o = it1->orientation;
				hash_type b = it1->signature;


				auto ls = storage[w].collections();
				auto begin = std::get<0>(ls);
				auto end = std::get<1>(ls);
				if (begin == end)
					continue;

				float weight = storage[w].weight;
				for (auto it = begin; it != end; ++it) {
					auto img_id = it->image_id;

					auto& entry = candidates[img_id];
					float dori = o - it->orientation;
					if (dori < 0) dori += 360.0;
					int dori_idx = static_cast<int>(dori / 60);

					float dlogs = s - it->logscale;
					if (dlogs < 0) dlogs += 8.0;
					int dlogs_idx = static_cast<int>(dlogs);

					entry.ori_hist[dori_idx] += 1;
					entry.logs_hist[dlogs_idx] += 1;
				} // for collections
			}

			vector<tuple<unsigned int, float> > rets;
			rets.reserve(candidates.size());
			for (auto& x : candidates) {
				auto& hists = x.second;
				float maxval = std::max(*std::max_element(hists.ori_hist.begin(), hists.ori_hist.end()),
										*std::max_element(hists.logs_hist.begin(), hists.logs_hist.end()));
				rets.push_back(std::make_tuple(x.first, maxval));
			}

			std::sort( rets.begin(), rets.end(),
					[](const tuple<unsigned int, float>& x, const tuple<unsigned int, float>& y){
						return std::get<1>(x) >= std::get<1>(y);
					});
			return move(rets);
		}

	private:
		std::vector<InvertedIndexPostingList> storage;
};



}
#endif /* __INVERTED_INDEX_H__ */
