#ifndef VG_MAPPER_HPP_INCLUDED
#define VG_MAPPER_HPP_INCLUDED

#include <iostream>
#include <map>
#include <chrono>
#include <ctime>
#include "omp.h"
#include "xg.hpp"
#include "alignment.hpp"
#include "path.hpp"
#include "position.hpp"
#include "json2pb.h"
#include "graph.hpp"
#include "gcsa/internal.h"
#include "xg_position.hpp"
#include "utility.hpp"

namespace vg {

using namespace sdsl;

class Counter {
public:
    Counter(void);
    Counter(xg::XG* xidx);
    ~Counter(void);
    xg::XG* xgidx;
    void merge_from_files(const vector<string>& file_names);
    void load_from_file(const string& file_name);
    void save_to_file(const string& file_name);
    void load(istream& in);
    size_t serialize(std::ostream& out,
                     sdsl::structure_tree_node* s = NULL,
                     std::string name = "");
    void ensure_edit_tmpfile_open(void);
    void close_edit_tmpfile(void);
    void remove_edit_tmpfile(void);
    void make_compact(void);
    void make_dynamic(void);
    void add(const Alignment& aln, bool record_edits = true);
    size_t graph_length(void) const;
    size_t position_in_basis(const Position& pos) const;
    string pos_key(size_t i) const;
    string edit_value(const Edit& edit, bool revcomp) const;
    vector<Edit> edits_at_position(size_t i) const;
    size_t coverage_at_position(size_t i) const;
    void collect_coverage(const Counter& c);
    ostream& as_table(ostream& out, bool show_edits = true);
    ostream& show_structure(ostream& out); // debugging
    void write_edits(ostream& out) const; // for merge
private:
    bool is_compacted;
    // dynamic model
    gcsa::CounterArray coverage_dynamic;
    string edit_tmpfile_name;
    fstream tmpfstream;
    //map<size_t, map<string, int32_t> > edits;
    // compact model
    //size_t total_count; // sum of counts in coverage and edit coverage
    size_t edit_length;
    size_t edit_count;
    vlc_vector<> coverage_civ; // graph coverage (compacted coverage_dynamic)
    //csa_sada<enc_vector<>, 32, 32, sa_order_sa_sampling<>, isa_sampling<>, succinct_byte_alphabet<> > edit_csa;
    csa_wt<> edit_csa;
    // make separators that are somewhat unusual, as we escape these
    char delim1 = '\xff';
    char delim2 = '\xfe';
    //string delim_sub = string(1, '\xff');
    // double the delimiter in the string
    string escape_delim(const string& s, char d) const;
    string escape_delims(const string& s) const;
    // take each double delimiter back to a single
    string unescape_delim(const string& s, char d) const;
    string unescape_delims(const string& s) const;
};

// for making a combined matrix output and maybe doing other fun operations
class Counters : public vector<Counter> {
    void load(const vector<string>& file_names);
    ostream& as_table(ostream& out);
};

}

#endif