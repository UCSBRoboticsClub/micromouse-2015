#include "Songs.h"
#include "Globals.h"
#include <Arduino.h>
#include <cstring>
#include <cstdlib>


void playSong(const char *s)
{
    while (s != NULL)
    {
        s = std::strchr(s, 'f');
        if (s == NULL) break;
        int f = std::strtol(++s, const_cast<char**>(&s), 10);
        if (s == NULL)  break;
        s = std::strchr(s, 't');
        if (s == NULL)  break;
        int t = std::strtol(++s, const_cast<char**>(&s), 10);

        if (f > 0)
            tone(buzzerPin, f);
        else
            noTone(buzzerPin);
        
        delay(t);
    }
    
    noTone(buzzerPin);
}


const char recorder[] = "f622t2 f698t168 f0t209 f698t107 f0t29 f698t121 f0t8 f698t617 f0t166 f494t2 f740t160 f0t66 f698t166 f659t2 f0t64 f698t39 f659t88 f698t20 f0t21 f784t8 f740t10 f0t84 f698t2 f740t119 f698t16 f659t23 f0t80 f698t74 f740t33 f784t21 f740t21 f587t2 f0t135 f784t33 f740t139 f370t2 f0t72 f659t166 f0t107 f698t248 f659t4 f0t59 f698t135 f494t2 f0t63 f622t2 f698t86 f0t105 f698t111 f0t119 f311t2 f587t80 f0t166 f587t63 f0t162 f659t117 f0t119 f659t63 f698t113 f0t230 f370t2 f698t328 f659t2 f0t588 f587t64 f523t2 f0t47 f587t123 f659t113 f698t236 f740t27 f784t43 f831t31 f880t94 f831t51 f784t51 f831t105 f880t96 f831t21 f784t766 f740t8 f0t480 f392t2 f587t63 f554t2 f0t88 f659t80 f698t78 f0t47 f698t68 f740t25 f784t43 f554t2 f0t88 f784t2 f831t29 f880t16 f932t4 f988t8 f932t2 f0t35 f932t18 f880t207 f0t35 f1047t31 f988t50 f932t44 f988t53 f1047t26 f831t2 f880t264 f932t98 f988t109 f932t105 f988t31 f932t33 f0t492 f554t2 f622t14 f0t76 f831t47 f880t33 f831t39 f698t2 f0t70 f831t100 f740t2 f0t121 f988t27 f1047t12 f988t2 f0t125 f698t2 f988t98 f932t29 f880t21 f831t18 f784t96 f831t35 f880t119 f932t682 f880t12 f932t338 f880t29 f932t492 f988t23 f932t744 f880t10";

const char startup[] = "f523t125 f587t125 f659t125 f698t125";

/* C4 262 
 * D4 294
 * E4 330
 * F4 349
 * G4 392
 * A5 440
 * A#5 466
 * B5 494
 * C5 523
 * D5 587
 * E5 659
 * F5 698
 * G5 784
 */

const char mortalkombat[] = "f440t250 f440t250 f523t250 f440t250 f587t250 f440t250 f659t250 f587t250 f523t250 f523t250 f659t250 f523t250 f784t250 f523t250 f659t250 f523t250 f392t250 f392t250 f494t250 f392t250 f523t250 f392t250 f587t250 f523t250 f349t250 f349t250 f440t250 f349t250 f523t250 f349t250 f587t250 f523t250 | f440t250 f440t250 f523t250 f440t250 f587t250 f440t250 f659t250 f587t250 f523t250 f523t250 f659t250 f523t250 f784t250 f523t250 f659t250 f523t250 f392t250 f392t250 f494t250 f392t250 f523t250 f392t250 f587t250 f523t250 f349t250 f349t250 f440t250 f349t250 f523t250 f349t250 f587t250 f523t250 | f440t250 f0t125 f440t250 f0t125 f440t250 f0t125 f440t250 f0t125 f392t250 f523t250 f440t250 f0t125 f440t250 f0t125 f440t250 f0t125 f440t250 f0t125 f392t250 f330t250 f440t250 f0t125 f440t250 f0t125 f440t250 f0t125 f440t250 f0t125 f392t250 f523t250 f440t250 f0t125 f440t250 f0t125 f440t250 f440t125 f440t125 f0t125 f440t125 f440t125 f0t375 | f440t250 f0t125 f440t250 f0t125 f440t250 f0t125 f440t250 f0t125 f392t250 f523t250 f440t250 f0t125 f440t250 f0t125 f440t250 f0t125 f440t250 f0t125 f392t250 f330t250 f440t250 f0t125 f440t250 f0t125 f440t250 f0t125 f440t250 f0t125 f392t250 f523t250 f440t250 f0t125 f440t250 f0t125 f440t250 f440t125 f440t125 f0t125 f440t125 f440t125 f0t375 | f440t125 f659t150 f0t100 f440t125 f523t150 f0t100 f440t125 f466t150 f0t100 f440t125 f523t150 f0t100 f440t125 f466t125 f392t150 f0t100 f440t125 f659t150 f0t100 f440t125 f523t150 f0t100 f440t125 f466t150 f0t100 f440t125 f523t150 f0t100 f440t125 f466t125 f392t150 f0t100 f440t125 f659t150 f0t100 f440t125 f523t150 f0t100 f440t125 f466t150 f0t100 f440t125 f523t150 f0t100 f440t125 f466t125 f392t150 f0t100 f440t125 f659t150 f0t100 f440t125 f523t150 f0t100 f392t125 f392t150 f0t100 f392t125 f440t150 f0t100 f440t250 f0t250 | f440t125 f659t150 f0t100 f440t125 f523t150 f0t100 f440t125 f466t150 f0t100 f440t125 f523t150 f0t100 f440t125 f466t125 f392t150 f0t100 f440t125 f659t150 f0t100 f440t125 f523t150 f0t100 f440t125 f466t150 f0t100 f440t125 f523t150 f0t100 f440t125 f466t125 f392t150 f0t100 f440t125 f659t150 f0t100 f440t125 f523t150 f0t100 f440t125 f466t150 f0t100 f440t125 f523t150 f0t100 f440t125 f466t125 f392t150 f0t100 f440t125 f659t150 f0t100 f440t125 f523t150 f0t100 f392t125 f392t150 f0t100 f392t125 f440t150 f0t100 f440t250 f0t250 | f622t250 f0t250 f622t250 f0t250 f622t250 f0t250 f622t250 f0t250 | f587t3875 f0t125 f523t3875 f0t125 f587t3875 f0t125 f523t3875 f0t125";
