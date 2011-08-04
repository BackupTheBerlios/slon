import os
import sys
import getopt

from pyplusplus import module_builder

def main():
    opts, args = getopt.getopt(sys.argv[1:], 'I:C:F:')

    sourceDir = args[0]
    gccxmlPath = args[1]
    compiler = ""
    includePaths = []
    for o,p in opts:
        if o in ['-I', '--include']:
            includePaths.append(p)
        elif o in ['-C', '--compiler']:
            compiler = p
        elif o in ['-F', '--compiler-flags']:
            flags = "--gccxml-cxxflags \"" + p.replace('\\','')[2:] + "\"";

    print flags

    mb = module_builder.module_builder_t( files = [sourceDir + '/slon/Scene/Node.h'], 
                                          gccxml_path = gccxmlPath,
                                          cflags = flags,
                                          include_paths = includePaths,
                                          compiler = compiler )

    mb.write_module( os.path.join( sourceDir + '/src/scene/Python/PyNode.cpp' ) )
    
if __name__ == "__main__":
    main()
