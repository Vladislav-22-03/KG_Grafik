using FishStore.Logic.DtoModels.Filters;
using Laba_3_OOP_FishStore.Features.DtoModels.Center;

namespace Laba_3_OOP_FishStore.Features.Interfaces.Managers
{
    public interface ICenterManager
    {
        void Create(EditCenter editCenter);

        void Update(EditCenter updateCenter);

        void Delete(Guid isnNode);

        CenterDto GetCenter(Guid isnNode);

        CenterDto[] GetListCenters(CenterFilterDto centerFilter);

    }
}
