#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* Space Invaders ROMs are structured in memory like this

    MEM
    $0000-$07ff: invaders.h argv[1]
    $0800-$0fff: invaders.g argv[2]
    $1000-$17ff: invaders.f argv[3]
    $1800-$1fff: invaders.e argv[4]
*/

int main(int argc, char *argv[])
{
    FILE *ofp, *ifp;
    uint8_t data[0x800];

    if (argc < 5) {
        fprintf(stderr, "Missing arguments...\n");
        exit(EXIT_FAILURE);
    }

    ofp = fopen("../../roms/invader.rom", "w+");
    if (!ofp)
        goto fopen_error;

    ifp = fopen("../../roms/invaders.h", "r"); 
    if (!ifp)
        goto fopen_error;
    fread(data, 1, 0x800, ifp);
    fwrite(data, 1, 0x800, ofp);
    fclose(ifp);

    ifp = fopen("../../roms/invaders.g", "r"); 
    if (!ifp)
        goto fopen_error;
    fread(data, 1, 0x800, ifp);
    fwrite(data, 1, 0x800, ofp);
    fclose(ifp);

    ifp = fopen("../../roms/invaders.f", "r"); 
    if (!ifp)
        goto fopen_error;
    fread(data, 1, 0x800, ifp);
    fwrite(data, 1, 0x800, ofp);
    fclose(ifp);
    
    ifp = fopen("../../roms/invaders.e", "r"); 
    if (!ifp)
        goto fopen_error;
    fread(data, 1, 0x800, ifp);
    fwrite(data, 1, 0x800, ofp);
    fclose(ifp);

    fclose(ofp);
    return 0;

fopen_error:
    fprintf(stderr, "fopen error\n");
    exit(EXIT_FAILURE);
}