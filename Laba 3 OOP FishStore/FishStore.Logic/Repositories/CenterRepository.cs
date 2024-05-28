using FishStore.Logic.Interfaces.Repositories;
using FishStore.Storage.Database;
using FishStore.Storage.Models;
using Microsoft.EntityFrameworkCore;

namespace FishStore.Logic.Repositories
{
    internal class CenterRepository : ICenterRepository
    {
        public Center Create(DataContext dataContext, Center center)
        {
            center.IsnNode = Guid.NewGuid();
            dataContext.Centers.Add(center);

            return center;
        }

        public Center Update(DataContext dataContext, Center center)
        {
            var centerDb = dataContext.Centers.FirstOrDefault(x => x.IsnNode == center.IsnNode)
                ?? throw new Exception($"Центр с идентификатором {center.IsnNode} не найден");

            centerDb.Code = center.Code;
            centerDb.Name = center.Name;

            return centerDb;
        }

        public void Delete(DataContext dataContext, Guid isnNode)
        {
            var centerDb = dataContext.Centers.FirstOrDefault(x => x.IsnNode == isnNode)
                ?? throw new Exception($"Центр с идентификатором {isnNode} не найден");

            dataContext.Centers.Remove(centerDb);
        }

        public Center GetById(DataContext dataContext, Guid isnNode)
        {
            var centerDb = dataContext.Centers.AsNoTracking().FirstOrDefault(x => x.IsnNode == isnNode)
                ?? throw new Exception($"Центр с идентификатором {isnNode} не найден");

            return centerDb;
        }

    }
}
