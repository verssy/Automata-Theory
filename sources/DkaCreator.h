#ifndef DKACREATOR_H
#define DKACREATOR_H

#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <map>
#include <set>


using namespace std;

class DkaCreator
{
public:
    struct Node {
        map<char, pair<int, int>> transitions{};
        string node_name = "q";
        bool is_exit_node = false;
        bool is_achievable = false;
        int index = -1;
    };

    struct TablePresentation {
        set<char> alphabet;
        set<string> states;
        set<string> end_states;
        map<int, vector<pair<char, string>>> transitions;
    };

    struct InputParams {
        string alphabet;
        string substr;
        int k;
        string filename;
    };

    DkaCreator(string alphabet, const string& substr, int k, string filename) {
        if (alphabet.length() == 0)
            throw runtime_error("DKA is obvious, add symbols in alphabet");
        if (k < 1)
            throw runtime_error("You're dumb, k < 1, really? :D");
        _filename = filename;
        if (_filename.find(".jpg", 0) == string::npos)
            _filename += ".jpg";
        set<char> letters;
        for (const auto& i : alphabet)
            letters.insert(i);
        _alphabet.reserve(letters.size());
        for (auto& i : letters)
            _alphabet.push_back(i);
        for (const auto& i : substr)
            if (letters.count(i) == 0)
                throw runtime_error("Substring symbol isn't in alphabet");
        _layers.resize(k, vector<Node>(substr.length() + 1));
        for (int i = 0; i < k; i++)
            for (auto& j : _alphabet)
                _layers[i].back().transitions[j] = { (i + 1) % _layers.size(), _layers[i].size() - 1 };
        for (int i = 0, j; i < _layers.size(); i++)
            for (j = 0; j < _layers[i].size(); j++) {
                _layers[i][j].node_name += ::to_string(i * _layers[0].size() + j);
                _layers[i][j].index = i * _layers[0].size() + j;
                table_presentation.states.insert(_layers[i][j].node_name);
            }
        for (int i = 0; i < k; i++)
            if ((substr.length() + i) % k == 0) {
                _layers[i].back().is_exit_node = true;
                table_presentation.end_states.insert(_layers[i].back().node_name);
                _exit_label = _layers[i].back().node_name;
                break;
            }
        for (int i = 0, j; i < k; i++)
            for (j = 0; j < _layers[i].size() - 1; j++) {
                _layers[i][j].transitions[substr[j]] = { i, j + 1 };
                for (const auto& symb : _alphabet)
                    if (_layers[i][j].transitions.count(symb) == 0) {
                        int pos_to = ret_index(substr, substr.substr(0, j).append(string(1, symb)));
                        _layers[i][j].transitions[symb] = { (i + (j - pos_to + 1)) % k, pos_to };
                    }
            }
        _layers[0][0].is_achievable = true;
        queue<pair<int, int>> q;
        q.push({ 0, 0 });
        while (!q.empty()) {
            auto top = q.front();
            q.pop();
            _layers[top.first][top.second].is_achievable = true;
            for (const auto& i : _layers[top.first][top.second].transitions)
                if (!_layers[i.second.first][i.second.second].is_achievable)
                    q.push({ i.second.first, i.second.second });
        }

        for (auto &i : _layers)
            for (auto &j : i)
                if (j.is_achievable) {
                    for (auto &ij : j.transitions)
                        table_presentation.transitions[j.index].push_back({ij.first, _layers[ij.second.first][ij.second.second].node_name});
                }
        for (auto &i : _alphabet)
            table_presentation.alphabet.insert(i);
        inputParams = {alphabet, substr, k, filename};
    }

    bool check(const string& chain) {
        for (const auto& i : chain)
            if (find(begin(_alphabet), end(_alphabet), i) == end(_alphabet))
                return 0;
        pair<int, int> curr_pos = { 0, 0 };
        for (const auto& i : chain)
            curr_pos = _layers[curr_pos.first][curr_pos.second].transitions[i];
        if (_layers[curr_pos.first][curr_pos.second].is_exit_node)
            return 1;
        return 0;
    }

    TablePresentation get_table_presentation() {
        return table_presentation;
    }

    InputParams get_input_params() {
        return inputParams;
    }

    void gen_image() {
        std::ofstream out("g.gv");
        if (out.is_open()) {
            out << "digraph G{\n";
            for (auto& i : _layers)
                for (auto& j : i)
                    if (j.is_achievable)
                        for (auto& k : j.transitions)
                            out << j.node_name << "->" << _layers[k.second.first][k.second.second].node_name << "[label=\"" << k.first << "\"]\n";
            out << _exit_label << " [shape=doublecircle]\n";
            out << "q0 [shape=doublecircle]\n";
            out << "}\n";
            out.flush();
            out.close();
            string dot_command = "dot -Tjpg g.gv -o " + _filename;
            system(dot_command.c_str());
            system(_filename.c_str());
        }
    }

    string get_hist(string chain) {
        if (check(chain) == 0)
            return {};
        pair<int, int> curr_pos = { 0, 0 };
        string hist = "(q0, " + chain + ")|-";
        int ind = 1;
        for (const auto& i : chain) {
            curr_pos = _layers[curr_pos.first][curr_pos.second].transitions[i];
            hist += "(" + _layers[curr_pos.first][curr_pos.second].node_name + ", " + chain.substr(ind++) + ")|-";
        }
        return hist;
    }

private:
    int ret_index(string substr, string chain) {
        int res = 0, bad = 0;
        for (int i = 0, j; i < min(substr.length(), chain.length()); i++, bad = 0) {
            for (j = 0; j < i + 1; j++)
                if (substr[j] != chain[chain.length() - 1 - i + j]) {
                    bad = 1;
                    break;
                }
            res = max(res, (i + 1) * (bad == 0));
        }
        return res;
    }

    TablePresentation table_presentation;
    InputParams inputParams;

    vector<vector<Node>> _layers;
    string _exit_label;
    string _alphabet;
    string _filename;
};

#endif // DKACREATOR_H
