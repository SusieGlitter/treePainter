#include "tree.h"

Tree::Tree()
{
    root=new Node;
    root->depth=0;
}

Tree::~Tree()
{
    delete root;
}

void Tree::clear(Node *now)
{
    if(now==root)
        while(layers.empty()==false)
            layers.pop_back();
    for(auto son:now->sons)
    {
        clear(son);
        if(son!=nullptr)
            delete son;
    }
    while(now->sons.empty()==false)
        now->sons.pop_back();
}

Node *Tree::newTree(QString s)
{
    QStringList lines=s.split("\n");
    lines.removeAll("");
    std::map<QString,int> m;
    std::vector<Node *>nodes;
    int mapcnt=0;
    m[""]=mapcnt++;
    Node *newRoot=new Node;
    nodes.push_back(newRoot);
    for(auto line:lines)
    {
        while(line.length()!=0&&line[line.length()-1]==' ')
            line=line.sliced(0,line.length()-1);
        while(line.length()!=0&&line[0]==' ')
            line=line.sliced(1,line.length()-1);
        if(line.length()==0)
            continue;
        if(line.contains(' '))
        {
            if(line.split(QChar(' ')).length()!=2)
                return nullptr;
            QString s1=line.split(QChar(' '))[0];
            QString s2=line.split(QChar(' '))[1];
            if(s1.length()==0||s2.length()==0)
                return nullptr;
            // qDebug()<<s1<<"!"<<s2;
            if(m.count(s1)==0)
            {
                m[s1]=mapcnt++;
                Node *temp=new Node;
                temp->name=s1;
                nodes.push_back(temp);
            }
            if(m.count(s2)==0)
            {
                m[s2]=mapcnt++;
                Node *temp=new Node;
                temp->name=s2;
                nodes.push_back(temp);
            }
            Node *node1=nodes[m[s1]],*node2=nodes[m[s2]];
            if(node1->fat==nullptr)
            {
                node1->fat=newRoot;
                newRoot->sons.push_back(node1);
            }
            if(node2->fat!=nullptr)
            {
                if(node2->fat!=newRoot)
                {
                    for(auto node:nodes)
                        delete node;
                    // qDebug()<<"oh no!\n";
                    return nullptr;
                }
                newRoot->sons.erase(std::find(newRoot->sons.begin(),newRoot->sons.end(),node2));
                node2->fat=nullptr;
            }
            node1->sons.push_back(node2);
            node2->fat=node1;
        }
        else
        {
            if(m.count(line)==0)
            {
                m[line]=mapcnt++;
                Node *temp=new Node;
                temp->name=line;
                nodes.push_back(temp);
            }
            Node *node=nodes[m[line]];
            if(node->fat==nullptr)
            {
                node->fat=newRoot;
                newRoot->sons.push_back(node);
            }
        }
    }
    return newRoot;
}

void Tree::updateTree(QString s)
{
    Node *newRoot=newTree(s);
    if(newRoot!=nullptr)
    {
        clear(root);
        root=newRoot;
    }

    adjust1(root);
    while(true)
    {
        while(layers.empty()==false)
            layers.pop_back();
        if(adjust2_1(root)==false)
            break;
    }
    adjust3_1(root);
    adjust3_2(root,canvasw/2-(xmax+xmin)/2,canvash/2-(ymax+ymin+hDis)/2);
}

void Tree::drawTree(QPainter &painter,Node *now)
{
    for(auto son:now->sons)
        drawTree(painter,son);
    if(now!=root&&now->fat!=root)
        painter.drawLine(now->x,now->y-r,now->fat->x,now->fat->y+r);
    if(now!=root)
        now->drawNode(painter,r);
}

void Tree::adjust1(Node *now)
{
    if(now==root)
    {
        now->x=180;
        now->y=0;
        now->depth=0;
    }
    double n=int(now->sons.size());
    for(int i=0;i<n;i++)
    {
        Node *son=now->sons[i];
        son->x=now->x-(n-1)/2*wDis-(n-1)*r+(wDis+2*r)*i;
        son->y=now->y+hDis+2*r;
        son->depth=now->depth+1;
        adjust1(son);
    }
}

bool Tree::adjust2_1(Node *now)
{
    if(now->depth>=int(layers.size()))
        layers.push_back(std::vector<Node *>());
    layers[now->depth].push_back(now);
    Node *last=layers[now->depth][layers[now->depth].size()-2];
    if(layers[now->depth].size()!=1&&(now->x-last->x)<(wDis+2*r))
    {
        adjust2_2(last,now);
        return true;
    }
    for(auto son:now->sons)
    {
        bool temp=adjust2_1(son);
        if(temp)
            return true;
    }
    return false;
}

void Tree::adjust2_2(Node *node1, Node *node2)
{
    double deltaX=((wDis+2*r)-(node2->x-node1->x))/2;
    while(node1->fat!=node2->fat)
    {
        node1=node1->fat;
        node2=node2->fat;
    }
    Node *subroot=node1->fat;
    double sgn=-1;
    for(auto son:subroot->sons)
    {
        if(son==node2)
            sgn=1;
        adjust3_2(son,deltaX*sgn,0);
    }
}

void Tree::adjust3_1(Node *now)
{
    if(now==root)
    {
        xmax=now->x;
        xmin=now->x;
        ymax=now->y;
        ymin=now->y;
    }
    xmax=std::max(xmax,now->x);
    xmin=std::min(xmin,now->x);
    ymax=std::max(ymax,now->y);
    ymin=std::min(ymin,now->y);
    for(auto son:now->sons)
    {
        adjust3_1(son);
    }
}

void Tree::adjust3_2(Node *now,double deltaX,double deltaY)
{
    now->x+=deltaX;
    now->y+=deltaY;
    for(auto son:now->sons)
    {
        adjust3_2(son,deltaX,deltaY);
    }
}
