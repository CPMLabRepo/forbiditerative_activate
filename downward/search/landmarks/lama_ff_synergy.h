#ifndef LAMA_FF_SYNERGY_H
#define LAMA_FF_SYNERGY_H

#include "../heuristic.h"
#include "exploration.h"
#include "landmarks_count_heuristic.h"

class LamaFFSynergy {
    class HeuristicProxy : public Heuristic {
    protected:
        LamaFFSynergy *synergy;
        bool is_first_proxy;

        virtual void initialize();
        virtual int get_heuristic_value() = 0;
	virtual bool is_dead_end() = 0;
	virtual void get_preferred_operators(std::vector<const Operator *> &result) = 0;
    public:
        HeuristicProxy(LamaFFSynergy *synergy_);
        virtual ~HeuristicProxy() {}

	virtual bool reach_state(const State& parent_state, const Operator& op,
				 const State& state) {
	    return synergy->reach_state(parent_state, op, state);
	}

        virtual int compute_heuristic(const State& state) {
            if(is_first_proxy)
                synergy->compute_heuristics(state);
            return get_heuristic_value();
        }
    };

    class FFHeuristicProxy : public HeuristicProxy {
	virtual bool is_dead_end() {
	    return synergy->ff_dead_end;
	}
        virtual int get_heuristic_value() {
            return synergy->ff_heuristic_value;
        }
	virtual void get_preferred_operators(std::vector<const Operator *> &result) {
	    synergy->get_ff_preferred_operators(result);
	}
    public:
        FFHeuristicProxy(LamaFFSynergy *synergy_) : HeuristicProxy(synergy_) {}
    };

    class LamaHeuristicProxy : public HeuristicProxy {
	virtual bool is_dead_end() {
	    return synergy->lama_dead_end;
	}
        virtual int get_heuristic_value() {
            return synergy->lama_heuristic_value;
        }
	virtual void get_preferred_operators(std::vector<const Operator *> &result) {
	    synergy->get_lama_preferred_operators(result);
	}
    public:
        LamaHeuristicProxy(LamaFFSynergy *synergy_) : HeuristicProxy(synergy_) {}
    };

    friend class HeuristicProxy;
    friend class LamaHeuristicProxy;
    friend class FFHeuristicProxy;

    LamaHeuristicProxy lama_heuristic_proxy;
    FFHeuristicProxy ff_heuristic_proxy;
    LandmarksCountHeuristic* lama_heuristic;
    Exploration* exploration;
    bool lm_pref;
    bool lm_admissible;
    std::vector<const Operator *> lama_preferred_operators;
    std::vector<const Operator *> ff_preferred_operators;
    bool initialized;

    void initialize() {
	// Value change only serves to determine first proxy.
        initialized = true;
    }

    bool reach_state(const State& parent_state, const Operator& op,
		     const State& state);
        
    void compute_heuristics(const State&);
    void get_lama_preferred_operators(std::vector<const Operator *> &result);
    void get_ff_preferred_operators(std::vector<const Operator *> &result);
public:
    LamaFFSynergy(bool lm_pref_, bool lm_admissible_);
    ~LamaFFSynergy() {}

    int lama_heuristic_value;
    int ff_heuristic_value;
    bool lama_dead_end;
    bool ff_dead_end;

    Heuristic *get_ff_heuristic_proxy() {
        return &ff_heuristic_proxy;
    }

    Heuristic *get_lama_heuristic_proxy() {
        return &lama_heuristic_proxy;
    }
};

#endif