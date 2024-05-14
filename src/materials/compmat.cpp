
/*
    pbrt source code is Copyright(c) 1998-2016
                        Matt Pharr, Greg Humphreys, and Wenzel Jakob.

    This file is part of pbrt.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */


// materials/compmat.cpp*
#include "materials/compmat.h"
#include "materials/matte.h"
#include "spectrum.h"
#include "reflection.h"
#include "paramset.h"
#include "texture.h"
#include "interaction.h"

namespace pbrt {

// MixMaterial Method Definitions
void CompMaterial::ComputeScatteringFunctions(SurfaceInteraction *si,
                                             MemoryArena &arena,
                                             TransportMode mode,
                                             bool allowMultipleLobes) const {
    if (si->MaterialID == 0) {
        m1->ComputeScatteringFunctions(si, arena, mode, allowMultipleLobes);
    } 
    else if (si->MaterialID == 1) {
        m2->ComputeScatteringFunctions(si, arena, mode, allowMultipleLobes);
    }
    else if (si->MaterialID == 2) {
        m3->ComputeScatteringFunctions(si, arena, mode, allowMultipleLobes);
    }
    else if (si->MaterialID == 3) {
        m4->ComputeScatteringFunctions(si, arena, mode, allowMultipleLobes);
    }
}

CompMaterial *CreateCompMaterial(const TextureParams &mp,
                               const std::shared_ptr<Material> &m1,
                               const std::shared_ptr<Material> &m2,
                               const std::shared_ptr<Material> &m3,
                               const std::shared_ptr<Material> &m4) {
    return new CompMaterial(m1, m2, m3, m4);
}

}  // namespace pbrt
