#ifndef HYPERGRAPH_DEFINES_H_INCLUDED
#define HYPERGRAPH_DEFINES_H_INCLUDED


namespace xray {
namespace editor {
namespace controls{

namespace hypergraph{

public enum class connection_type : int {input=0, output=1, inout=2 };
public enum class point_align : int {none=0, left=1, right=2, top=3, bottom=4};
public enum class nodes_visibility : int {all=0, active_chain=1 };
public enum class nodes_movability : int {movable=0, fixed=1, by_nodes_movability = 2 };
public enum class view_mode: int {brief=0, /*medium=1,*/ full=2 };
public enum class nodes_link_style : int {arrow=0, line=1 };

bool is_output(connection_type t);
bool is_input(connection_type t);

ref class node;
ref class link;
typedef Generic::List<node^>				nodes;
typedef Generic::List<link^>				links;

public delegate void position_changed (node^ pnode);

} //namespace hypergraph
} //namespace controls
} //namespace editor
} //namespace xray

#endif //HYPERGRAPH_DEFINES_H_INCLUDED