using FishStore.Logic.DtoModels.Filters;
using FishStore.Storage.Database;
using FishStore.Storage.Models;

namespace FishStore.Logic.Interfaces.Services
{
    public interface ICenterService
    {
        IQueryable<Center> GetCenterQueryble(DataContext dataContext, CenterFilterDto filter, bool asNoTracking);
    }
}
