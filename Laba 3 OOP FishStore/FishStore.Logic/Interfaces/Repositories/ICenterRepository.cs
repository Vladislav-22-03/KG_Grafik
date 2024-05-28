using FishStore.Storage.Database;
using FishStore.Storage.Models;

namespace FishStore.Logic.Interfaces.Repositories
{
    public interface ICenterRepository
    {
        Center Create(DataContext dataContext, Center center);

        Center Update(DataContext dataContext, Center center);

        void Delete(DataContext dataContext, Guid isnNode);

        Center GetById(DataContext dataContext, Guid isnNode);
    }
}
