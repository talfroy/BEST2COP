# 0) Folder Structure 

SRG contains Best2cop as presented in Computer Network & NCA. This version of Best2cop relies on the SR Graph to compute DCLC-SR paths. The conversion to the SRG is done beforehand, before the algorithm itself is run. In this version, Best2cop explores the SR Graph itself.

LCA contains a version of best2cop which does not explore the SR Graph itself, but rather relies on it to convert the paths it explores in their respective segment lists. 

Finally, 2D contains a version of best2cop which does not consider Segment Routing at all. Thus, there is no SR Graph, nor any conversions. The goal of this version is to evaluation the performance of Best2cop as a constrained-path computation algorithm in a non-SR context. 

# 1) Compilation

```
make
```

This will compile the whole main program. You are now able to use it !


# 2) Best2cop

```
./best2cop [option] [mandadory parameters]
```

## 2.1) Mandadory parameters

For each of these parameters you will need to choose one of them

### 2.1.1) File to load

Adding 

```
--file [path_to_file]
```

To choose the topology file to load

### 2.1.2) Type of file loaded

```
--topo or --sr
```

Use --topo if the file represents the initial topology and --sr if the file represents the SR graph

### 2.1.3) Node identifiers

```
--id or --labels
```

Use --id if the node are identified by a number, and --labels if the node are identified by a label

### 2.1.4) Type of running

```
--all-nodes or --src [src_id]
```

Use --all-nodes option if you want to run best2cop with each node as source node, or --src if you want to run it using a specific source node

## 2.2) Options

```
--accu [accuracy]
```

Set the delay accuracy to 10^(-accuracy)

```
--output [output_file]
```

Write the results in the given output file (created if not exists)

```
--cstr1 [c1]
```

Modify the first constraint (--cstr2 to modify the second)

```
--full
```

Remove the SEGMAX limit

# 3) Owners


Jean-Romain Luttringer (Jr.luttringer@unistra.fr)
Pascal Merindol (merindol@unistra.fr)
Bramas Quentin (bramas@unistra.fr)
Cristel Pelsser (pelsser@unistra.fr)
Francois Clad (fclad@cisco.com)
Thomas Alfroy (thomas.alfroy@etu.unistra.fr)
