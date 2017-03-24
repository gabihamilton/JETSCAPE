// -----------------------------------------
// JetScape (modular/task) based framework
// Intial Design: Joern Putschke (2017)
//                (Wayne State University)
// -----------------------------------------
// License and Doxygen-like Documentation to be added ...

//Test PartonShower with graphh from GTL
#include "PartonShower.h"
#include <sstream>
#include <fstream>
#include <iomanip>

node PartonShower::new_vertex(shared_ptr<VertexBase> v)
 {
   node n=graph::new_node();
   vMap[n]=v;
   return n;
 }

void PartonShower::new_parton(node s, node t, shared_ptr<Parton> p)
{
  edge e=graph::new_edge(s,t);
  pMap[e]=p;
}

void PartonShower::save_node_info_handler (ostream *o, node n) const
{
  *o<<"x "<<vMap[n]->x_in().x()<<endl;
  *o<<"y "<<vMap[n]->x_in().y()<<endl;
  *o<<"z "<<vMap[n]->x_in().z()<<endl;
  *o<<"t "<<vMap[n]->x_in().t()<<endl;
}


void PartonShower::save_edge_info_handler (ostream *o, edge e) const
{

  *o<<"label "<<pMap[e]->plabel()<<endl;
  *o<<"pid "<<pMap[e]->pid()<<endl;
  *o<<"pstat "<<pMap[e]->pstat()<<endl;
  *o<<"pT "<<pMap[e]->pt()<<endl;
  *o<<"eta "<<pMap[e]->eta()<<endl;
  *o<<"phi "<<pMap[e]->phi()<<endl;
  *o<<"E "<<pMap[e]->e()<<endl;
  
}

void PartonShower::pre_clear_handler()
{
  VERBOSESHOWER(8);
  edge_iterator eIt, eEnd;
  for (eIt = edges_begin(), eEnd = edges_end(); eIt != eEnd; ++eIt)
    {
      pMap[*eIt]=nullptr;
    }
  
  node_iterator nIt, nEnd;
  for (nIt = nodes_begin(), nEnd = nodes_end(); nIt != nEnd; ++nIt)
    {
      vMap[*nIt]=nullptr;
    }
}

void PartonShower::PrintNodes()
{
  node_iterator nIt, nEnd;
  ostringstream os;
  
  for (nIt = nodes_begin(), nEnd = nodes_end(); nIt != nEnd; ++nIt)
    {      
      os<<*nIt<<"="<<vMap[*nIt]->x_in().t()<<" ";
    }

  //cout<<os.str()<<endl;
  VERBOSESHOWER(8)<<os.str();
}

void PartonShower::PrintEdges()
{
  edge_iterator eIt, eEnd;
  ostringstream os;
  
  for (eIt = edges_begin(), eEnd = edges_end(); eIt != eEnd; ++eIt)
    {
      os<<*eIt<<"="<<pMap[*eIt]->pt()<<" ";
    }
  
  VERBOSESHOWER(8)<<os.str();
}

// To be extended to store all infos like this in GML
// or store vectore of partons and vertecies with relevant graph infos
// to construct the graph later .... (TBD)
// These handlers are needed if one wants to use load and GML files ...
// Obsolete with new JetScape reader ...

void PartonShower::load_edge_info_handler (edge e, GML_pair *read)
{
  VERBOSESHOWER(8)<<"Load edge ... "<<e;
  struct GML_pair* tmp = read;

  double pT,eta,phi,E;
    
  while (tmp) {
    //printf ("*KEY* : %s \n", tmp->key);
    if (((string) (tmp->key)).find("pT")<1)
      pT=tmp->value.floating;
    if (((string) (tmp->key)).find("eta")<1)
      eta=tmp->value.floating;
    if (((string) (tmp->key)).find("phi")<1)
      phi=tmp->value.floating;
    if (((string) (tmp->key)).find("E")<1)
      E=tmp->value.floating;

    tmp = tmp->next;
  }

  pMap[e]=make_shared<Parton>(0,21,0,pT,eta,phi,E);
}

void PartonShower::load_node_info_handler (node n, GML_pair *read)
{
  VERBOSESHOWER(8)<<"Load node ... "<<n;
  struct GML_pair* tmp = read;

  double x,y,z,t;
    
  while (tmp) {
    //printf ("*KEY* : %s %f \n", tmp->key, tmp->value.floating);
    if (((string) (tmp->key)).find("x")<1)
      x=tmp->value.floating;
    if (((string) (tmp->key)).find("y")<1)
      y=tmp->value.floating;
    if (((string) (tmp->key)).find("z")<1)
      z=tmp->value.floating;
    if (((string) (tmp->key)).find("t")<1)
      t=tmp->value.floating;

    tmp = tmp->next;
  }

  vMap[n]=make_shared<VertexBase>(x,y,z,t);
}

// use with graphviz (on Mac: brew install graphviz --with-app)
// dot GVfile.gv -Tpdf -o outputPDF.pdf

void PartonShower::SaveAsGV(string fName)
{
  ofstream gv; gv.open(fName.c_str());

  // Simple directed graph left->right in dot/gv format for usage with graphviz ...
  // nodes show (time) and arrows (pT)
  gv<<"digraph \"graph\" {"<<endl;
  gv<<endl;
  gv<<"rankdir=\"LR\";"<<endl;
  gv<<"node [shape=plaintext, fontsize=11];"<<endl; //, shape=circle]; //plaintext];
  gv<<"edge [fontsize=10];"<<endl;
  gv<<endl;
  //gv<<"0 -> 1"<<endl;
  node_iterator nIt, nEnd;

  int n=0;
  string label;
  string label2;
  
  for (nIt = nodes_begin(), nEnd = nodes_end(); nIt != nEnd; ++nIt)
    {
      label = ("[label=\""+to_string(n)+"(");
      label2 = ")\"];";
      stringstream stream;
      
      stream << fixed << setprecision(2) << (vMap[*nIt]->x_in().t());
      gv<<n<<" "<<label<<stream.str()<<label2<<endl;
      
      n++;
    }

  gv<<endl;
  
  edge_iterator eIt, eEnd;
  
  for (eIt = edges_begin(), eEnd = edges_end(); eIt != eEnd; ++eIt)
    {

      label = ("[label=\"(");    
      label2 = ")\"];";     
      stringstream stream;
      stream << fixed << setprecision(2) << (pMap[*eIt]->pt());
      
      gv<<to_string(eIt->source().id())+"->"+to_string(eIt->target().id())<<" "<<label<<stream.str()<<label2<<endl;
    }
  
  gv<<endl;
  gv<<"}"<<endl;
}