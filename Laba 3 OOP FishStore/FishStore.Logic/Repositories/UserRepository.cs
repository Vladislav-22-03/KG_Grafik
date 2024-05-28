using FishStore.Logic.Interfaces.Repositories;
using FishStore.Storage.Database;
using FishStore.Storage.Models;

namespace FishStore.Logic.Repositories
{
	public class UserRepository : IUserRepository
	{
		public User Create(DataContext dataContext, User user)
		{
			user.IsnNode = Guid.NewGuid();
			dataContext.Users.Add(user);

			return user;
		}

		//public Center Update(DataContext dataContext, Center center)
		//{
		//	var centerDb = dataContext.Centers.FirstOrDefault(x => x.IsnNode == center.IsnNode)
		//		?? throw new Exception($"Центр с идентификатором {center.IsnNode} не найден");

		//	centerDb.Code = center.Code;
		//	centerDb.Name = center.Name;

		//	return centerDb;
		//}

		//public void Delete(DataContext dataContext, Guid isnNode)
		//{
		//	var centerDb = dataContext.Centers.FirstOrDefault(x => x.IsnNode == isnNode)
		//		?? throw new Exception($"Центр с идентификатором {isnNode} не найден");

		//	dataContext.Centers.Remove(centerDb);
		//}

		//public Center GetById(DataContext dataContext, Guid isnNode)
		//{
		//	var centerDb = dataContext.Users.AsNoTracking().FirstOrDefault(x => x.IsnNode == isnNode)
		//		?? throw new Exception($"Центр с идентификатором {isnNode} не найден");

		//	return centerDb;
		//}
	}
}
