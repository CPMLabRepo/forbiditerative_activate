#include "pattern_generator_random.h"

#include "pattern_database.h"
#include "random_pattern.h"
#include "utils.h"

#include "../option_parser.h"
#include "../plugin.h"
#include "../task_proxy.h"

#include "../utils/logging.h"
#include "../utils/rng.h"
#include "../utils/rng_options.h"

#include <vector>

using namespace std;

namespace pdbs {
PatternGeneratorRandom::PatternGeneratorRandom(options::Options &opts)
    : max_pdb_size(opts.get<int>("max_pdb_size")),
      max_time(opts.get<double>("max_time")),
      bidirectional(opts.get<bool>("bidirectional")),
      verbosity(opts.get<utils::Verbosity>("verbosity")),
      rng(utils::parse_rng_from_options(opts)) {
}

PatternInformation PatternGeneratorRandom::generate(
    const shared_ptr<AbstractTask> &task) {
    if (verbosity >= utils::Verbosity::NORMAL) {
        utils::g_log << "Generating pattern using the Random Pattern algorithm."
                     << endl;
    }

    utils::Timer timer;
    vector<vector<int>> cg_neighbors = compute_cg_neighbors(
        task, bidirectional);
    TaskProxy task_proxy(*task);
    vector<FactPair> goals = get_goals_in_random_order(task_proxy, *rng);

    Pattern pattern = generate_random_pattern(
        max_pdb_size,
        max_time,
        verbosity,
        rng,
        task_proxy,
        goals[0].var,
        cg_neighbors);

    PatternInformation result(task_proxy, pattern);
    if (verbosity >= utils::Verbosity::NORMAL) {
        dump_pattern_generation_statistics(
            "Random Pattern",
            timer.stop(),
            result);
    }
    return result;
}

static shared_ptr<PatternGenerator> _parse(options::OptionParser &parser) {
    parser.document_synopsis(
        "Random Pattern",
        "This pattern generator implements the 'single randomized "
        "causal graph' algorithm described in experiments of the the paper"
        + get_rovner_et_al_reference() +
        "It computes a pattern by performing a simple random walk on the "
        "causal graph, starting from the given random goal variable, and "
        "including all visited variables.");
    add_random_pattern_implementation_notes_to_parser(parser);
    parser.add_option<int>(
        "max_pdb_size",
        "maximum number of states in the final pattern database (possibly "
        "ignored by a singleton pattern consisting of a single goal variable)",
        "1000000",
        Bounds("1", "infinity"));
    parser.add_option<double>(
        "max_time",
        "maximum time in seconds for the pattern generation",
        "infinity",
        Bounds("0.0", "infinity"));
    add_random_pattern_bidirectional_option_to_parser(parser);
    utils::add_verbosity_option_to_parser(parser);
    utils::add_rng_options(parser);

    Options opts = parser.parse();
    if (parser.dry_run()) {
        return nullptr;
    }

    return make_shared<PatternGeneratorRandom>(opts);
}

static Plugin<PatternGenerator> _plugin("random_pattern", _parse);
}