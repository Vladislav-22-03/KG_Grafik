using AutoMapper;
using FishStore.Storage.Models;
using Laba_3_OOP_FishStore.Features.DtoModels.Center;

namespace Laba_3_OOP_FishStore.Features
{
    public class CenterMapper : Profile
    {
        public CenterMapper()
        {
            CreateMap<Center, EditCenter>()
                .ForMember(x => x.IsnNode, o => o.MapFrom(x => x.IsnNode));

            CreateMap<EditCenter, Center>();
        }
    }
}
